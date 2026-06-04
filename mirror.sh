#!/bin/sh

git pull
git submodule update --init

cd ../
if [ ! -d itmo-highload-mirror ]; then
	git clone git@github.com:fadyat/itmo-highload-mirror.git
fi

cd itmo-highload-mirror && git pull
rsync -av --delete --exclude '.git' --exclude '.gitmodules' ../itmo-highload/ .

case "$(git remote get-url origin)" in
	*itmo-highload-mirror*) ;;
	*) echo "refusing to push: origin is not the mirror ($(git remote get-url origin))" >&2; exit 1 ;;
esac

# flatten submodules: drop gitlink entries so their files commit as plain content
git ls-files -s | grep '^160000 ' | cut -f2 | while IFS= read -r p; do
	git rm --cached -q -- "$p"
done

git add -A
git commit -m 'feat: synced'
git push
