FROM ubuntu:16.04
MAINTAINER Dilawar Singh <dilawar.s.rajput@gmail.com>

# If you are behind proxy,  uncomment the following lines with appropriate
# values.
ENV http_proxy http://proxy.ncbs.res.in:3128
ENV https_proxy http://proxy.ncbs.res.in:3128

RUN apt-get update 
RUN apt-get install -y wget x11-apps xorg
RUN apt-get install -y python-matplotlib python-numpy
RUN apt-get install -y python-pip  python-networkx graphviz python-scipy
RUN apt-get install -y python-pygraphviz
RUN pip install python-libsbml
    
# Install the package from STABLE channel.
RUN wget -nv https://download.opensuse.org/repositories/home:moose/xUbuntu_16.04/Release.key -O /tmp/Release.key
RUN apt-key add - < /tmp/Release.key
RUN sh -c "echo 'deb http://download.opensuse.org/repositories/home:/moose/xUbuntu_16.04/ /' > /etc/apt/sources.list.d/moose.list"
RUN apt-get update && apt-get install -y --allow-unauthenticated moose

RUN adduser --disabled-password --gecos '' mooser

USER mooser
ENV HOME /home/mooser
CMD /usr/bin/xterm
