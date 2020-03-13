# Run this script from top directory
# docker build . --file .ci/Dockerfile --tag bhallalab/build-wheels:$(date +%s)
FROM bhallalab/python-wheels:latest
MAINTAINER Dilawar Singh <dilawar.s.rajput@gmail.com>
WORKDIR /root
COPY . /root/moose-core/
# RUN ./build_wheels.sh
CMD [ "./build_wheels.sh" ]
