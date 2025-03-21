export function setAuthToken(token: string) {
  localStorage.setItem("token", token)
}

export function clearAuthToken() {
  localStorage.removeItem("token")
}

export function getAuthToken(): string | null {
  return localStorage.getItem("token")
}
