#!/bin/sh
# 1. Delete binary file
rm -rf *.img *.bin *.gz *.csv *.tgz dbbackup dbdata dbuser

# 2. Make db area directories
mkdir dbbackup dbdata dbuser

# 3. db contents copy to dbdata and dbbackup area without hidden folder(.svn)
rsync -avz --exclude=.svn ./dbdata_source/* ./dbdata/
rsync -avz --exclude=.svn ./dbbackup_source/* ./dbbackup/
rsync -avz --exclude=.svn ./dbuser_source/* ./dbuser/

PRODUCT_NAME=`echo $1`

# 3. copy "$PRODUCT_NAME"_gang.bin
mkdir src_images
cp ../output/"$PRODUCT_NAME"_gang.bin ./src_images/"$PRODUCT_NAME"_gang.bin
cp ../output/"$PRODUCT_NAME"_gang_factory_loader.bin  ./src_images/factory.bin #copy factoryloader

# 4. Make ubifs
./.bin/mkfs.ubifs -m 2048 -e 124KiB -c 265 -q -r ./dbdata dbdata.img
./.bin/mkfs.ubifs -m 2048 -e 124KiB -c 265 -q -r ./dbbackup dbbackup.img
./.bin/mkfs.ubifs -m 2048 -e 124KiB -c 496 -q -r ./dbuser dbuser.img

# 5. Make gang image
./all_nand_gang_"$PRODUCT_NAME".sh ./config/nand_img_"$PRODUCT_NAME".cfg "$PRODUCT_NAME"_fullgang

# 6. Delete temp data
rm -rf *.bin dbbackup dbdata dbuser src_images
