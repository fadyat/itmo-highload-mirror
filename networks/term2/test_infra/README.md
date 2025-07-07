This is readme about testing infrastructure setup.

# Runner

Computer networks course has dedicated runner VM. As of spring 2025 runner works on 84.252.143.244.
Runner picks up tasks only with tag computer-networks. Our runner is group runner and belongs to computer-networks group.

You can find the runner setup [here](https://gitlab.manytask.org/groups/computer-networks/-/runners).

## Docker images

Script `test_infra/dockerfiles/build.py` should build all required images and tag them localy.

The list of images is:
* containerlab is described in `test_infra/dockerfiles/containerlab.docker` should be tagged as containerlab.
* testenv is described in `test_infra/dockerfiles/testenv.docker` should be tagged as testenv.
* frrenv is described in `test_infra/dockerfiles/frrenv.docker` should be tagged as frrenv.

## Pushing images to registry

To push images to the GitLab registry, you can use the following command:
```bash
build.py --registry gitlab.manytask.org:5050/computer-networks/public-2025-spring
```

This will:
1. Build all required images
2. Tag them with the registry prefix
3. Push them to the specified registry

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

[Manytask documentation](https://github.com/manytask/manytask/blob/0946aaa1bf11c5113d4114e65368aba0c6f5885f/README.md?plain=1#L147) about the method.

```
export TESTER_TOKEN={value}
curl -X POST -H "Authorization: Bearer $TESTER_TOKEN" \
        -H "Content-type: application/x-yaml" --data-binary "@.manytask.yml" \
        "https://networks.manytask.org/api/update_config"
```

## Curl to set arbitrary score to a student

[Manytask documentation](https://github.com/manytask/manytask/blob/0946aaa1bf11c5113d4114e65368aba0c6f5885f/README.md?plain=1#L145) about the method.

```
export TESTER_TOKEN={value}
export TASK_NAME={some_name} # e.g. static_routing
export USER_NAME={gitlab_user_name} # e.g. feldsherov
export SCORE={some scode} # e.g. 50
curl -X POST https://networks.manytask.org/api/report \
  -d "token=${TESTER_TOKEN}" \
  -d "task=${TASK_NAME}" \
  -d "username=${USER_NAME}" \
  -d "score=${SCORE}"
```

# Gitlab

## Workflow

Repository is large enough and main brach is protected by setting [here](https://gitlab.manytask.org/computer-networks/computer-networks/-/settings/repository), none can directly push to main.

Expected worklow is classic feature branch workflow.
Develop new thing in feature brach, get green CI, create merge request, merge it.

## CI/CD Configuration

### releaser-ci.yml

This file controls the test infrastructure release process through GitLab CI/CD. Usage of this file is configured in the GitLab project settings:

- Path: Settings > CI/CD > Generic > CI/CD configuration file [link](https://gitlab.manytask.org/computer-networks/computer-networks/-/settings/ci_cd).
- File: `.releaser-ci.yml`


### DOCKER_REGISTRY_DEPLOY_PASSWORD and DOCKER_REGISTRY_DEPLOY_USER

Pipeline uses DOCKER_REGISTRY_DEPLOY_PASSWORD and DOCKER_REGISTRY_DEPLOY_USER variable to publish images. This variable is configured [here](https://gitlab.manytask.org/groups/computer-networks/-/settings/ci_cd).

Registry token is issued in [computer networks deploy tokens](https://gitlab.manytask.org/groups/computer-networks/-/settings/repository).