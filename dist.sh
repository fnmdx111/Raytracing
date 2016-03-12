rm -rf $1_zc2324
mkdir $1_zc2324

cp src/* $1_zc2324/
cp include/* $1_zc2324/
cp Makefile_ $1_zc2324/Makefile

tar cf $1_zc2324.tar $1_zc2324

