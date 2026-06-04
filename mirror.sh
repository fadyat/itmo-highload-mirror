#!/bin/sh

git pull
git submodule update --init

cd ../
if [ ! -d itmo-highload-mirror ]; then
	git clone git@github.com:fadyat/itmo-highload-mirror.git
fi

cd itmo-highload-mirror && git pull
rsync -av --delete --exclude '.git/' --exclude '.gitmodules' ../itmo-highload/ .

case "$(git remote get-url origin)" in
	*itmo-highload-mirror*) ;;
	*) echo "refusing to push: origin is not the mirror ($(git remote get-url origin))" >&2; exit 1 ;;
esac

git add .
git commit -m 'feat: synced'
git push
