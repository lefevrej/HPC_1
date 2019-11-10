LD_LIBRARY_PATH=/usr/local/lib
D_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib/
export LD_LIBRARY_PATH
echo "Sobels excecution times with different option opti" > sobel_out.txt
echo "" &>> sobel_out.txt
echo "classic" > sobel_out.txt
g++ `pkg-config --cflags opencv4` sobels.cpp `pkg-config --libs opencv4` -lpthread -o sobels
echo "" &>> sobel_out.txt
./sobels &>> sobel_out.txt
echo "----------------------------------------------------------------------------------" &>> sobel_out.txt

echo "" &>> sobel_out.txt
echo "O1" &>> sobel_out.txt
echo "" &>> sobel_out.txt
g++ `pkg-config --cflags opencv4` sobels.cpp `pkg-config --libs opencv4` -lpthread -o sobels -O1
./sobels &>> sobel_out.txt
echo "----------------------------------------------------------------------------------" &>> sobel_out.txt

echo "" &>> sobel_out.txt
echo "O2" &>> sobel_out.txt
echo "" &>> sobel_out.txt
g++ `pkg-config --cflags opencv4` sobels.cpp `pkg-config --libs opencv4` -lpthread -o sobels -O2
./sobels &>> sobel_out.txt
rm sobels
cat sobel_out.txt
