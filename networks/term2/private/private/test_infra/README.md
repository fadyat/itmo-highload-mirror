This is readme about testing infrastructure setup.

# Runner

Computer networks course has dedicated runner VM. As of spring 2025 runner works on 84.252.143.244.
Runner picks up tasks only with tag computer-networks. Our runner is group runner and belongs to computer-networks group.

You can find the runner setup [here](https://gitlab.manytask.org/groups/computer-networks/-/runners).

## Docker images

Currently course does not use docker registry, this means, that all required images should be build and tagged on runner.

Script `test_infra/dockerfiles/build.sh` should build all required images and tag them.

The list of images is:
* containerlab is described in `test_infra/dockerfiles/containerlab.docker` should be tagged as containerlab.
* testenv is described in `test_infra/dockerfiles/testenv.docker` should be tagged as testenv.


## Runner setup

Runner should have sudoless docker access.
Testing heavily relies on access to docker.


# Container lab

All routing tasks relies on container lab. Here is a hew commands, which can be useful for debugging.

## Run container lab locally in docker

```
docker run -it --rm --privileged \
    --network host \
    -v /var/run/docker.sock:/var/run/docker.sock \
    -v /var/run/netns:/var/run/netns \
    -v /etc/hosts:/etc/hosts \
    -v /var/lib/docker/containers:/var/lib/docker/containers \
    --pid="host" \
    -v "${PWD}:/repo" \
    containerlab bash
```
## Deploy topology

`containerlab deploy -t lab.yml`

## Destroy topology
`containerlab destroy -t static_routing/lab.yml -c`


# Interaction with Manytask

## TESTER_TOKEN

You can find tester token [here](https://gitlab.manytask.org/groups/computer-networks/-/settings/ci_cd) in computer networks group settings. (Expand vairables)

## Curl to update .manytask.yml

```
export TESTER_TOKEN={value}
curl -X POST -H "Authorization: Bearer $TESTER_TOKEN" \
        -H "Content-type: application/x-yaml" --data-binary "@.manytask.yml" \
        "https://networks.manytask.org/api/update_config"
```