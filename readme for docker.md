#run these in the individual directory's src folder


#build Ubuntu

docker build -t vsdk-x.x.x-on-ubuntu -f ../Dockerfile-Ubuntu/Dockerfile .
docker run -it --rm vsdk-x.x.x-on-ubuntu

#macOS
docker build --platform linux/amd64 -t vsdk-x.x.x-on-ubuntu -f Dockerfile-Ubuntu/Dockerfile .
docker run -it --platform linux/amd64  vsdk-x.x.x-on-ubuntu

#list all images
docker images -a

#remove images
docker rmi 


