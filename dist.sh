rm -rf $1_zc2324
mkdir $1_zc2324
mkdir $1_zc2324/src
mkdir $1_zc2324/include

cp -r src/ $1_zc2324/src
cp -r include/ $1_zc2324/include
cp Makefile $1_zc2324/

tar cf $1_zc2324.tar $1_zc2324

