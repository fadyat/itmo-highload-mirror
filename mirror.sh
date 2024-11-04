#!/bin/sh

git pull
git submodule update --init

cd ../
if [ ! -d itmo-highload-mirror ]; then
	git clone git@github.com:fadyat/itmo-highload-mirror.git
fi

cd itmo-highload-mirror && git pull
rsync -av --delete --exclude '**/.git/' --exclude '.gitmodules' ../itmo-highload/ .
git add .
git commit -m 'feat: synced'
git push 
