/** @type {import('ts-jest').JestConfigWithTsJest} **/
export default {
  testEnvironment: "jest-environment-jsdom",
  transform: {
    "^.+\.tsx?$": ["babel-jest", {}],
  },
  preset: 'ts-jest/presets/default-esm',
  "roots": [
    "<rootDir>",
    "<rootDir>/app"
  ],
  "modulePaths": [
    "<rootDir>",
    "<rootDir>/app"
  ],
  "moduleDirectories": [
    "node_modules"
  ],
  setupFiles: [`<rootDir>/jest-shim.ts`],
};