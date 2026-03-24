# Docker Notes

Each sample has a `Dockerfile-Ubuntu/Dockerfile`, but those Dockerfiles are legacy helpers and have not been revalidated after the repo moved to a shared repo-level `SDK/` layout.

Current recommendation:

- prefer the native CMake build from the host
- if you use Docker, make the repo-level `SDK/` available inside the container
- treat the sample Dockerfiles as starting points, not as guaranteed up-to-date build recipes

If you adapt one of them, use the repo root as the build context so the container can see:

- `SDK/`
- `cmake/`
- the sample source directory

