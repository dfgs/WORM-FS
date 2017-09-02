FROM centos:latest
MAINTAINER Marc GUICHARD "Marc.Guichard@IPC.com"

ENV REFRESHED_AT 2016-30-11
RUN yum -y -q upgrade
RUN yum -y groupinstall 'Development Tools'
RUN yum -y install fuse
RUN yum -y install fuse-devel
RUN yum -y install samba

ENV REFRESHED_AT 2016-01-12
#ADD worm-1.0.tar.gz /root
#WORKDIR /root/worm-1.0
#RUN ./configure
#RUN make
#RUN make install

RUN mkdir /mnt/WORM
COPY WORM /usr/local/bin
COPY WORM.conf /etc

VOLUME ["/opt/audit"]

COPY start.pl /root
EXPOSE 139 445
CMD ["--user","worm:worm:1000","--share","Archive:/Archive/:worm"]
ENTRYPOINT ["/root/start.pl"]
