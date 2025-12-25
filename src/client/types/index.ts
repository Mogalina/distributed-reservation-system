export interface User {
  userId: string;
  token: string;
}

export interface AuthResponse {
  userId: string;
  token: string;
  error?: string;
  message?: string;
}

export interface AuthRequestData {
  userId: string;
  nationalId: string;
  firstName?: string;
  lastName?: string;
}
