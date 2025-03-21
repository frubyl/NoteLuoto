import { type RouteConfig, index, route } from "@react-router/dev/routes";

export default [
  index("routes/home.tsx"),
  route("login", "routes/sign-in.tsx"),
  route("register", "routes/sign-up.tsx")
] satisfies RouteConfig;
