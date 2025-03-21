import { ChatPage } from "pages/chat";

import type { Route } from "./+types/home";
import { redirect } from "react-router";
import { getAuthToken } from "shared/auth";

export async function clientLoader({ request }: Route.ClientLoaderArgs) {
  const url = new URL(request.url)
  const returnTo = url.pathname + url.search

  if (!getAuthToken()) {
    return redirect(`/login?returnTo=${encodeURIComponent(returnTo)}`)
  }
}

export default ChatPage;
