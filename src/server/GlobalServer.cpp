/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GlobalServer.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 15:12:14 by vblanc            #+#    #+#             */
/*   Updated: 2026/03/09 23:13:59 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "GlobalServer.hpp"
#include "Cgi.hpp"
#include "HTTPStatusException.hpp"

bool keepRunningServer = true;

static void sigHandler(int signal)
{
    if (signal == SIGINT)
        keepRunningServer = false;
}

GlobalServer::GlobalServer(GlobalConfig &globalConfig) : _globalConfig(globalConfig)
{
    try
    {
        signal(SIGINT, sigHandler);
        this->setupGlobalServer();

        this->loopServer();
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << e.what() << std::endl;
        throw std::exception();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        throw std::exception();
    }
}

GlobalServer::~GlobalServer()
{
    std::string pad(4, ' ');
    std::cout << MAGENTA BOLD "~GlobalServer():" DEFAULT << std::endl;

    for (std::map<int, ClientContext *>::iterator it = this->_clientContexts.begin(); it != this->_clientContexts.end(); it++)
    {
        std::cout << pad << MAGENTA "Closing client fd " << it->first << DEFAULT << std::endl;
        close(it->first);
        delete it->second;
    }
    std::cout << std::endl;

    for (std::map<int, CgiContext *>::iterator it = this->_cgiContexts.begin(); it != this->_cgiContexts.end(); it++)
    {
        std::cout << pad << MAGENTA "Closing CGI fd " << it->first << " (killing pid " << it->second->pid << ")" DEFAULT << std::endl;
        close(it->first);
        kill(it->second->pid, SIGKILL);
        waitpid(it->second->pid, NULL, 0);
        delete it->second;
    }
    std::cout << std::endl;

    for (std::size_t i = 0; i < this->_servers.size(); i++)
    {
        std::cout << pad << MAGENTA "For server \'" << this->_globalConfig.getServerConfig().at(i).getServerName().at(0) << "\' [" << i << "]:" DEFAULT << std::endl;
        this->_servers.at(i).closeServerSockets();
    }
    std::cout << DEFAULT;

    std::cout << pad << MAGENTA "Closing epoll fd (this->_epfd)" DEFAULT << std::endl;
    close(this->_epfd);

    signal(SIGINT, SIG_DFL);
}

void GlobalServer::setupGlobalServer()
{
    this->_epfd = epoll_create(1);
    if (this->_epfd < 0)
        throw(std::runtime_error(RED "epoll_create1() error" DEFAULT));

    std::vector<ServerConfig> serverConfig = this->_globalConfig.getServerConfig();
    for (std::size_t i = 0; i < serverConfig.size(); i++)
    {
        std::cout << BLUE + getTimeOfDay() + " [info] : Trying to create server ‘" ITALIC + serverConfig.at(i).getServerName().at(0) + DEFAULT BLUE "’..." DEFAULT << std::endl;
        _servers.push_back(Server(serverConfig.at(i), this->_epfd));
        std::cout << GREEN + getTimeOfDay() + " [ok] : Created server ‘" ITALIC + serverConfig.at(i).getServerName().at(0) + DEFAULT GREEN "’ sucessfully!" DEFAULT << std::endl;
    }

    for (std::size_t i = 0; i < this->_servers.size(); i++)
    {
        std::map<int, ServerContext *> serverContext = this->_servers.at(i).getServerContext();
        for (std::map<int, ServerContext *>::iterator it = serverContext.begin(); it != serverContext.end(); it++)
            this->_serversConfig.insert(std::make_pair(it->first, this->_servers.at(i).getServerConfig()));
    }

    std::cout << GREEN + getTimeOfDay() + " [ok] : Created server(s) sucessfully!" DEFAULT << std::endl;
}

void GlobalServer::loopServer()
{
    std::cout << BLUE + getTimeOfDay() + " [info] : Waiting events..." DEFAULT << std::endl;

    while (keepRunningServer)
    {
        struct epoll_event events[MAX_EPOLL_WAIT_EVENTS];
        int n = epoll_wait(this->_epfd, events, MAX_EPOLL_WAIT_EVENTS, 100);

    this->closeOldClientConnections();
        this->closeTimedOutCgi();

        if (n < 0)
            continue;
        for (int i = 0; i < n; i++)
        {
            EpollContext *context = static_cast<EpollContext *>(events[i].data.ptr);

            if (ServerContext *serverContext = dynamic_cast<ServerContext *>(context))
            {
                if (events[i].events & EPOLLIN)
                    this->handleNewClientConnection(serverContext->fd);
            }
            else if (ClientContext *clientContext = dynamic_cast<ClientContext *>(context))
            {
                if (events[i].events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
                    this->handleCloseConnection(clientContext);
                else
                {
                    if (events[i].events & EPOLLIN)
                        this->handleReading(clientContext);
                    if (events[i].events & EPOLLOUT)
                        this->handleWriting(clientContext);
                }
            }
            else if (CgiContext *cgiContext = dynamic_cast<CgiContext *>(context))
                this->handleCgiEvent(cgiContext);
        }
    }
}

static void resetClientContext(ClientContext *clientContext)
{
    clientContext->state = READING_HEADERS;

    clientContext->recvBuffer.clear();
    clientContext->isChunkedRequest = false;
    clientContext->currentUnchunkedIndex = 0;
    clientContext->expectedBodySize = 0;
    clientContext->bodyStartIndex = 0;

    clientContext->sendBuffer.clear();
    clientContext->sendBufferIndex = 0;
}

static ClientContext *newClientContext(int clientSocket, int serverFd)
{
    ClientContext *clientContext;

    try
    {
        clientContext = new ClientContext;
    }
    catch (const std::exception &e)
    {
        std::cerr << RED + getTimeOfDay() + " [emerg] : Unexpected error during `new`: \"" << e.what() << "\"" << DEFAULT << std::endl;
        return (NULL);
    }

    clientContext->fd = clientSocket;

    resetClientContext(clientContext);

    clientContext->serverFd = serverFd;
    clientContext->keepAlive = false;
    clientContext->lastActive = time(NULL);

    return (clientContext);
}

void GlobalServer::handleNewClientConnection(int &serverFd)
{
    int clientSocket;
    struct sockaddr_in clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);

    while ((clientSocket = accept(serverFd, (struct sockaddr *)&clientAddr, &clientAddrSize)) != -1)
    {
        if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) < 0)
        {
            close(clientSocket);
            return;
        }
        ClientContext *clientContext = newClientContext(clientSocket, serverFd);
        if (clientContext == NULL)
        {
            close(clientSocket);
            continue;
        }

        this->_clientContexts[clientSocket] = clientContext;

        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLRDHUP;
        ev.data.ptr = clientContext;

        if (epoll_ctl(this->_epfd, EPOLL_CTL_ADD, clientSocket, &ev))
        {
            this->_clientContexts.erase(clientSocket);
            close(clientSocket);
            std::cerr << RED "epoll_ctl() error" DEFAULT << std::endl;
            return;
        }

        std::cout << GREEN + getTimeOfDay() + " [ok] : Accepted new connexion from port ‘" << ntohs(clientAddr.sin_port) << "’ (as fd " << clientSocket << ")" DEFAULT << std::endl;
    }
}

static void enableEPOLLOUT(int &epfd, ClientContext *clientContext)
{
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP;
    ev.data.ptr = clientContext;

    if (epoll_ctl(epfd, EPOLL_CTL_MOD, clientContext->fd, &ev))
        throw std::runtime_error(RED "epoll_ctl() error" DEFAULT);
}

static void disableEPOLLOUT(int &epfd, ClientContext *clientContext)
{
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLRDHUP;
    ev.data.ptr = clientContext;

    if (epoll_ctl(epfd, EPOLL_CTL_MOD, clientContext->fd, &ev))
        throw std::runtime_error(RED "epoll_ctl() error" DEFAULT);
}

static std::string buildSimpleErrorResponse(const std::string &status, const std::string &message)

{
    const std::string body = message + '\n';
    std::string response;

    response += "HTTP/1.1 " + status + " " + message + "\r\n";
    response += "Content-Type: text/plain\r\n";
    response += "Content-Length: " + toString(body.size()) + "\r\n";
    response += "Connection: close\r\n";
    response += "\r\n";
    response += body;
    return (response);
}

static void setClientErrorResponse(ClientContext *clientContext, const std::string &status, const std::string &message)

{
    clientContext->sendBuffer = buildSimpleErrorResponse(status, message);
    clientContext->sendBufferIndex = 0;
    clientContext->keepAlive = false;
    clientContext->state = READY_TO_SEND;
}

static void handleHeaders(ClientContext *clientContext)
{
    std::size_t pos = clientContext->recvBuffer.find("\r\n\r\n");
    if (pos != std::string::npos)
    {
        clientContext->bodyStartIndex = pos + 4;
        clientContext->currentUnchunkedIndex = clientContext->bodyStartIndex;

        std::size_t posTransfer = clientContext->recvBuffer.find("Transfer-Encoding:");
        if (posTransfer != std::string::npos)
        {
            std::string transferEncodingLine = clientContext->recvBuffer.substr(posTransfer, clientContext->recvBuffer.find("\r\n", posTransfer) - posTransfer);
            if (transferEncodingLine.find("chunked") != std::string::npos)
                clientContext->isChunkedRequest = true;
        }
        if (clientContext->isChunkedRequest == false)
        {
            std::string contentLengthFormat = "Content-Length:";
            std::size_t posContent = clientContext->recvBuffer.find(contentLengthFormat);
            if (posContent != std::string::npos)
            {
                std::size_t startSize = posContent + contentLengthFormat.size();
                std::size_t endSize = clientContext->recvBuffer.find("\r\n", startSize);

                std::stringstream ss(clientContext->recvBuffer.substr(startSize, endSize - startSize));
                ss >> clientContext->expectedBodySize;

                if (!ss.eof() || ss.fail())
                    throw(HttpStatusException("400", "Bad Request"));

                if (clientContext->expectedBodySize > MAX_HEADER_SIZE)
                    throw(HttpStatusException("431", "Request Header Fields Too Large"));
            }
        }

        clientContext->state = READING_BODY;

        if (clientContext->isChunkedRequest == false && clientContext->expectedBodySize == 0)
            clientContext->state = READY_TO_SEND;
    }
}

void unchunkBody(ClientContext *clientContext)
{
    std::string chunkedBuffer = clientContext->recvBuffer.substr(clientContext->currentUnchunkedIndex);
    std::size_t pos;
    std::string unchunkedBuffer;

    pos = 0;
    while (pos < chunkedBuffer.size())
    {
        std::size_t chunkSizeEndIndex = chunkedBuffer.find("\r\n", pos);
        if (chunkSizeEndIndex == std::string::npos)
            throw(HttpStatusException("400", "Bad Request 1"));

        std::string chunkSizeStr = chunkedBuffer.substr(pos, chunkSizeEndIndex - pos);

        // Magic number: 8 is for max 0xFFFFFFFF = 4GB
        if (chunkSizeStr.empty() || chunkSizeStr[0] == ' ' || chunkSizeStr.size() > 8)
            throw HttpStatusException("400", "Bad Request 2");

        if (chunkSizeStr.size() >= 2 && chunkSizeStr[0] == '0' &&
            (chunkSizeStr[1] == 'x' || chunkSizeStr[1] == 'X'))
            throw HttpStatusException("400", "Bad Request");

        char *endPtr = NULL;
        errno = 0;
        std::size_t chunkSize = std::strtoul(chunkSizeStr.c_str(), &endPtr, 16);
        if (*endPtr != '\0' || errno == ERANGE)
            throw HttpStatusException("400", "Bad Request 3");

        std::size_t chunkBufferStartIndex = chunkSizeEndIndex + 2;
        if (chunkBufferStartIndex + chunkSize > chunkedBuffer.size())
            throw(HttpStatusException("400", "Bad Request 4"));

        std::size_t chunkBufferEndIndex = chunkedBuffer.find("\r\n", chunkBufferStartIndex);
        if (chunkBufferEndIndex != chunkBufferStartIndex + chunkSize)
            throw(HttpStatusException("400", "Bad Request 5"));

        unchunkedBuffer.append(chunkedBuffer, chunkBufferStartIndex, chunkBufferEndIndex - chunkBufferStartIndex);
        pos = chunkBufferEndIndex + 2;
    }

    clientContext->recvBuffer = clientContext->recvBuffer.substr(0, clientContext->currentUnchunkedIndex) + unchunkedBuffer;
    clientContext->currentUnchunkedIndex = clientContext->recvBuffer.size();
}

static void handleBody(ClientContext *clientContext, std::string &pathRequest, ServerConfig &serverConfig)
{
    if (clientContext->isChunkedRequest == true)
    {
        if (clientContext->recvBuffer.find("0\r\n\r\n") != std::string::npos)
            clientContext->state = READY_TO_SEND;

        unchunkBody(clientContext);
    }

    if (clientContext->isChunkedRequest == false && clientContext->expectedBodySize > 0)
    {
        if (clientContext->recvBuffer.size() - clientContext->bodyStartIndex == clientContext->expectedBodySize)
            clientContext->state = READY_TO_SEND;
        else if (clientContext->recvBuffer.size() - clientContext->bodyStartIndex > clientContext->expectedBodySize)
            throw(HttpStatusException("413", "Payload Too Large"));
    }
    else if (clientContext->isChunkedRequest == false)
        throw(HttpStatusException("400", "Bad Request"));

    if (pathRequest.empty())
    {
        std::string firstLine = clientContext->recvBuffer.substr(0, clientContext->recvBuffer.find("\r\n"));
        std::size_t methodEnd = firstLine.find(' ');
        if (methodEnd == std::string::npos)
            throw(HttpStatusException("400", "Bad Request"));
        std::size_t pathEnd = firstLine.find(' ', methodEnd + 1);
        if (pathEnd == std::string::npos)
            throw(HttpStatusException("400", "Bad Request"));
        pathRequest = firstLine.substr(methodEnd + 1, pathEnd - (methodEnd + 1));
    }

    if (serverConfig.isValidLocationPath(pathRequest))
    {
        long long locationClientMaxBodySize = serverConfig.getLocationConfigByPath(pathRequest).getClientMaxBodySize();
        if (clientContext->recvBuffer.size() - clientContext->bodyStartIndex > static_cast<std::size_t>(locationClientMaxBodySize))
            throw(HttpStatusException("413", "Payload Too Large"));
    }
    else if (clientContext->recvBuffer.size() - clientContext->bodyStartIndex > static_cast<std::size_t>(serverConfig.getClientMaxBodySize()))
        throw(HttpStatusException("413", "Payload Too Large"));
}

void GlobalServer::handleReading(ClientContext *clientContext)
{
    if (clientContext->state != READING_HEADERS && clientContext->state != READING_BODY)
    {
        std::cout << YELLOW DARKEN + getTimeOfDay() + " [debug] : Trying to read but ‘clientContext->state != *READING*’ (" << clientContext->state << ")..." DEFAULT << std::endl;
        return;
    }

    ssize_t r;
    char buf[RECV_BUFFER_SIZE];
    std::string pathRequest;

    r = recv(clientContext->fd, buf, RECV_BUFFER_SIZE, 0);
    if (r > 0)
    {
        for (ssize_t i = 0; i < r; i++)
            clientContext->recvBuffer.push_back(buf[i]);

        try
        {
            if (clientContext->state == READING_HEADERS || clientContext->state == READING_BODY)
            {
                if (clientContext->state == READING_HEADERS)
                    handleHeaders(clientContext);
                if (clientContext->state == READING_BODY)
                    handleBody(clientContext, pathRequest, this->_serversConfig.at(clientContext->serverFd));
            }
            else
                throw(HttpStatusException("400", "Bad Request"));
        }
        catch (const HttpStatusException &e)
        {
            setClientErrorResponse(clientContext, e.getStatus(), e.getMessage());
            try
            {
                enableEPOLLOUT(this->_epfd, clientContext);
            }
            catch (const std::exception &)
            {
                this->handleCloseConnection(clientContext);
            }
            return;
        }
    }
    else
        throw std::runtime_error(RED "error ? r=" + toString(r) + " errno = " + toString(errno) + DEFAULT);

    std::cout << "\"" << clientContext->recvBuffer << "\"" << std::endl; // TODO: for debug (to delete)

    clientContext->lastActive = time(NULL);

    if (clientContext->state == READY_TO_SEND)
    {
        try
        {
            std::cout << YELLOW "Trying HTTPRequest()" DEFAULT << std::endl; // TODO: debug (to delete)
            HTTPRequest httpRequest(this->_serversConfig.at(clientContext->serverFd), clientContext->recvBuffer, clientContext->sendBuffer, this->_sessionManager);
            std::cout << YELLOW "Response generated by HTTPRequest()" DEFAULT << std::endl; // TODO: debug (to delete)

            enableEPOLLOUT(this->_epfd, clientContext);
        }
        catch (const CgiRequiredException &e)
        {
            std::cout << e.what() << std::endl;
            CgiRequestInfo const &cgiInfo = e.getCgiInfo();
            CgiContext *cgiCtx = executeCgi(cgiInfo, clientContext, this->_epfd);

            if (!cgiCtx)
            {
                setClientErrorResponse(clientContext, "502", "Bad Gateway");
                enableEPOLLOUT(this->_epfd, clientContext);
            }
            else
                this->_cgiContexts[cgiCtx->fd] = cgiCtx;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << std::endl;
        }
    }
}

void GlobalServer::handleWriting(ClientContext *clientContext)
{
    if (clientContext->state != READY_TO_SEND && clientContext->state != SENDING)
    {
        std::cout << YELLOW DARKEN + getTimeOfDay() + " [debug] : Trying to write but ‘clientContext->state != *SEND*’ (" << clientContext->state << ")..." DEFAULT << std::endl;
        return;
    }

    if (clientContext->state == READY_TO_SEND)
        clientContext->state = SENDING;

    ssize_t s;
    while (clientContext->sendBufferIndex < static_cast<ssize_t>(clientContext->sendBuffer.size()))
    {
        s = send(clientContext->fd, clientContext->sendBuffer.c_str() + clientContext->sendBufferIndex, clientContext->sendBuffer.size() - clientContext->sendBufferIndex, MSG_NOSIGNAL);

        if (s > 0)
            clientContext->sendBufferIndex += s;
        else if (s == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
        {
            enableEPOLLOUT(this->_epfd, clientContext);
            return;
        }
        else
        {
            throw std::runtime_error(RED "error ? s=" + toString(s) + " errno = " + toString(errno) + DEFAULT);
            this->handleCloseConnection(clientContext);
        }
    }

    if (clientContext->sendBufferIndex == static_cast<ssize_t>(clientContext->sendBuffer.size()))
    {
        disableEPOLLOUT(this->_epfd, clientContext);
        if (clientContext->keepAlive == false)
            this->handleCloseConnection(clientContext);
        else
            resetClientContext(clientContext);
    }
    else
        throw std::runtime_error(RED "Unexpected error during handleWriting(): clientContext->sendBufferIndex != clientContext->sendBuffer.size()" DEFAULT);
}

void GlobalServer::handleCloseConnection(ClientContext *clientContext)
{
    if (this->_clientContexts.count(clientContext->fd) == 0)
    {
        std::cout << MAGENTA + getTimeOfDay() + " [debug] : Trying to close client fd " << clientContext->fd << " but is already closed" DEFAULT << std::endl;
        return;
    }

    std::vector<int> cgiToClean;
    for (std::map<int, CgiContext *>::iterator it = this->_cgiContexts.begin(); it != this->_cgiContexts.end(); ++it)
        if (it->second->client == clientContext)
            cgiToClean.push_back(it->first);
    for (std::size_t i = 0; i < cgiToClean.size(); ++i)
    {
        std::map<int, CgiContext *>::iterator found = this->_cgiContexts.find(cgiToClean.at(i));
        if (found != this->_cgiContexts.end())
            cleanupCgi(found->second);
    }

    std::cout << MAGENTA + getTimeOfDay() + " [debug] : Close connexion fd " << clientContext->fd << DEFAULT << std::endl;

    if (epoll_ctl(this->_epfd, EPOLL_CTL_DEL, clientContext->fd, NULL))
        throw(std::runtime_error(RED "epoll_ctl() HERE error" DEFAULT));

    close(clientContext->fd);
    this->_clientContexts.erase(clientContext->fd);
    delete clientContext;
}

void GlobalServer::closeOldClientConnections()
{
    std::vector<int> clientContextsToClose;

    for (std::map<int, ClientContext *>::iterator it = this->_clientContexts.begin(); it != this->_clientContexts.end(); it++)
    {
    if ((time(NULL) - it->second->lastActive) > TIMEOUT_OLD_CONNECTIONS)
            clientContextsToClose.push_back(it->first);
    }

    for (std::size_t i = 0; i < clientContextsToClose.size(); i++)
    {
        std::cout << MAGENTA + getTimeOfDay() + " [debug] : Closing old client fd " << clientContextsToClose.at(i) << DEFAULT << std::endl;
    this->handleCloseConnection(this->_clientContexts.at(clientContextsToClose.at(i)));
    }
}

void GlobalServer::cleanupCgi(CgiContext *cgiContext)

{
    if (!this->_cgiContexts.count(cgiContext->fd))
        return;
    if (epoll_ctl(this->_epfd, EPOLL_CTL_DEL, cgiContext->fd, NULL) ^ 0)
        std::cerr << RED "epoll_ctl() error during CGI cleanup" DEFAULT << std::endl;
    close(cgiContext->fd);
    kill(cgiContext->pid, SIGKILL);
    waitpid(cgiContext->pid, NULL, 0);
    this->_cgiContexts.erase(cgiContext->fd);
    delete (cgiContext);
}

void GlobalServer::handleCgiEvent(CgiContext *cgiContext)

{
    char buf[4096];

    unsigned long r = read(cgiContext->fd, buf, sizeof(buf));
    if (r > 0)
    {
        cgiContext->output.append(buf, r);
    }
    else if (!r)
    {
        std::cout << GREEN + getTimeOfDay() + " [ok] : CGI finished normally for fd " << cgiContext->fd << DEFAULT << std::endl;

        std::string response;
        response += "HTTP/1.1 200 OK\r\n";
        response += "Content-Type: text/plain\r\n";
        response += "Content-Length: " + toString(cgiContext->output.size()) + "\r\n";
        response += "Connection: close\r\n";
        response += "\r\n";
        response += cgiContext->output;

        ClientContext *client = cgiContext->client;
        client->sendBuffer = response;
        client->sendBufferIndex = 0;
        client->state = READY_TO_SEND;
        enableEPOLLOUT(this->_epfd, client);
        cleanupCgi(cgiContext);
    }
    else if (errno == EAGAIN || errno == EWOULDBLOCK)
        return;
    else
    {
        std::cerr << RED + getTimeOfDay() + " [error] : Error reading from CGI fd " << cgiContext->fd << DEFAULT << std::endl;

        ClientContext *client = cgiContext->client;
        client->sendBuffer = buildSimpleErrorResponse("502", "Bad Gateway");
        client->sendBufferIndex = 0;
        client->state = READY_TO_SEND;
        enableEPOLLOUT(this->_epfd, client);
        cleanupCgi(cgiContext);
    }
}

void GlobalServer::closeTimedOutCgi()

{
    std::vector<int> cgiContextsToClose;

    for (std::map<int, CgiContext *>::iterator it = this->_cgiContexts.begin(); it != this->_cgiContexts.end(); it++)
    {
        if ((time(NULL) - it->second->startTime) > CGI_TIMEOUT)
            cgiContextsToClose.push_back(it->first);
    }
    for (unsigned long i = 0; i < cgiContextsToClose.size(); i++)
    {
        std::cout << MAGENTA + getTimeOfDay() + " [debug] : CGI timeout for fd " << cgiContextsToClose.at(i) << DEFAULT << std::endl;

        CgiContext *cgiContext = this->_cgiContexts.at(cgiContextsToClose.at(i));

        ClientContext *client = cgiContext->client;
        if (!client || this->_clientContexts.count(client->fd) == 0)
        {
            cleanupCgi(cgiContext);
            continue;
        }
        client->sendBuffer = buildSimpleErrorResponse("504", "Gateway Timeout");
        client->sendBufferIndex = 0;
        client->state = READY_TO_SEND;
        enableEPOLLOUT(this->_epfd, client);

        cleanupCgi(cgiContext);
    }
}
