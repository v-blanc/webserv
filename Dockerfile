FROM ubuntu:22.04

# Installe les outils nécessaires
RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    make \
    gdb \
    valgrind \
    vim \
    curl \
    && rm -rf /var/lib/apt/lists/*

# Définit le répertoire de travail
WORKDIR /workspace

# Copie ton projet (optionnel, tu peux aussi utiliser un volume)
# COPY . /workspace

# Expose le port de ton webserv
EXPOSE 8080

# Commande par défaut
CMD ["/bin/bash"]