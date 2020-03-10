FROM opensuse/leap
MAINTAINER Dilawar Singh <dilawars@ncbs.res.in>

# Install dependencies.
RUN zypper install -y git cmake gcc gcc-c++ make \
      python3 python3-devel python3-setuptools python3-numpy-devel \
      && rm -rf /var/cache/zypp/packages/*
RUN zypper install -y gsl-devel  \
      && rm -rf /var/cache/zypp/packages/*
# These are required to run tests.
RUN zypper install -y python3-matplotlib python3-networkx graphviz python3-scipy  \
      && rm -rf /var/cache/zypp/packages/*
WORKDIR /home/root
# RUN git clone  https://github.com/dilawar/moose-core -b devel
# Run docker build from outside 
COPY . moose-core
RUN cd moose-core && python3 setup.py build test install 
CMD ["/usr/bin/python3", "-c", "'import moose;moose.test()'"]
