FROM quay.io/pypa/manylinux2014_x86_64
RUN yum install -y gcc gcc-c++ cmake make
RUN yum install -y epel-release
RUN yum install -y make gcc perl-core pcre-devel wget zlib-devel python3
RUN yum install -y https://packages.endpointdev.com/rhel/7/os/x86_64/endpoint-repo.x86_64.rpm
RUN yum install -y git
RUN yum install -y curl-devel expat-devel gettext-devel zlib-devel perl-ExtUtils-MakeMaker
RUN wget https://ftp.openssl.org/source/openssl-1.1.1k.tar.gz --no-check-certificate
RUN tar -xzvf openssl-1.1.1k.tar.gz
RUN cd openssl-1.1.1k; find ./ -type f -exec sed -i -e 's/\#\ define\ OPENSSL\_VERSION\_NUMBER/\#define\ OPENSSL\_VERSION\_NUMBER/g' {}; ./config --prefix=/usr --openssldir=/etc/ssl --libdir=lib no-shared zlib-dynamic; make; make install;
ADD "https://www.random.org/cgi-bin/randbyte?nbytes=10&format=h" skipcache
RUN git clone https://github.com/kouta-kun/duckdb-hexhammdist; cd duckdb-hexhammdist; git clone https://github.com/duckdb/duckdb; cd duckdb; git pull --tags && git checkout v0.5.1 && cd .. && make;
