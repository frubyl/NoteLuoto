import { POST } from "shared/api";
import type { LoginRequest } from "shared/api";

export type SignInResult = {
  status: number,
  access_token: string | undefined
}

export async function signIn(body: LoginRequest): Promise<SignInResult> {
  const { data, response } = await POST("/auth/login", { body: body });

  return {
    status: response.status,
    access_token: data?.access_token
  };
}
