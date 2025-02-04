# NoteLuoto frontend

## Usage
```bash
npm install
npm run dev
```

## Environment variables

| Name | Use |
| --- | --- |
| VITE_NOTELUOTO_BACKEND_URL | Backend API base url |
| VITE_USE_API_MOCKS | Set to enable [MSW](https://mswjs.io) api mocks, for testing without a backend |

## Project structure

Structured according to [Feature-Sliced design](https://feature-sliced.design/docs/get-started/overview).

| Folder | Contents |
| --- | --- |
| `app` | everything that makes the app run — routing, entrypoints, global styles, providers |
| `app/mocks` | API mock handlers |
| `pages` | App pages |
| `pages/{page}/api` | Data fetching, using `shared/api` |
| `pages/{page}/ui` | The page itself |
| `public` | Static files |
| `shared` | reusable functionality |
| `shared/api` | API client, schemas, models |
| `shared/config` | App configuration |
