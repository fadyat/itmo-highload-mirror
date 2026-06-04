#!/bin/sh

git pull
git submodule update --init

cd ../ || exit 1
if [ ! -d itmo-highload-mirror ]; then
	git clone git@github.com:fadyat/itmo-highload-mirror.git
fi

cd itmo-highload-mirror || exit 1

# only ever push to the public mirror, never to the source repo
case "$(git remote get-url origin)" in
	*itmo-highload-mirror*) ;;
	*) echo "refusing to push: origin is not the mirror ($(git remote get-url origin))" >&2; exit 1 ;;
esac

git pull

# sync the working tree from the source repo, preserving only the mirror's own root .git
rsync -av --delete --exclude '/.git' --exclude '.gitmodules' ../itmo-highload/ .

# strip every nested .git (submodule gitfiles & embedded repos) so submodule
# content is committed as plain public files instead of gitlink references
find . -mindepth 2 -name .git -exec rm -rf {} +

# drop stale submodule gitlink entries so their files get added as real content
git ls-files -s | grep '^160000 ' | cut -f2 | while IFS= read -r p; do
	git rm --cached -q -- "$p"
done

git add -A
git commit -m 'feat: synced'
git push
