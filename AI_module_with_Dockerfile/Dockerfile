FROM ubuntu:latest

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    pciutils lshw curl wget software-properties-common && \
    rm -rf /var/lib/apt/lists/*

RUN add-apt-repository ppa:deadsnakes/ppa -y && \
    apt-get update && \
    apt-get install -y python3.11 python3.11-venv python3.11-dev python3-pip && \
    rm -rf /var/lib/apt/lists/*

RUN update-alternatives --install /usr/bin/python3 python3 /usr/bin/python3.11 1

RUN curl -fsSL https://ollama.ai/install.sh | sh

COPY AI_module /AI_module

WORKDIR /AI_module

RUN python3 -m venv venv

RUN /AI_module/venv/bin/pip install --upgrade pip

RUN /AI_module/venv/bin/pip install --no-cache-dir -r requirements.txt

RUN if lspci | grep -i nvidia; then \
        apt-get update && apt-get install -y nvidia-driver-535 nvidia-utils-535; \
    elif lspci | grep -i amd; then \
        apt-get update && apt-get install -y amdgpu-pro; \
    else \
        echo "No supported GPU detected."; \
    fi

CMD ["/bin/bash", "-c", "ollama serve & /AI_module/venv/bin/python AI_module.py"]
