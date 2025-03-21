import createClient, { type Middleware } from "openapi-fetch";

import { backendBaseUrl } from "shared/config";
import type { paths } from "./v1";
import { getAuthToken } from "shared/auth";

const authMiddleware: Middleware = {
  async onRequest({ request, schemaPath }) {
    if (schemaPath == "/auth/login" || schemaPath == "/auth/register") {
      return undefined
    }

    let token = getAuthToken()
    if (!token) {
      return undefined
    }

    request.headers.set("Authentication", "Bearer " + token)
  }
}

const client = createClient<paths>({ baseUrl: backendBaseUrl });

client.use(authMiddleware)

export const { GET, POST, PUT, DELETE } = client;
