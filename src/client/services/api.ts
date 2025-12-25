import { AuthRequestData, AuthResponse } from '../types';

const API_URL = 'http://127.0.0.1:8080';

export const authRequest = async (endpoint: string, data: AuthRequestData): Promise<AuthResponse> => {
  try {
    const response = await fetch(`${API_URL}/auth/${endpoint}`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(data),
    });
    
    const result = await response.json();
    if (!response.ok) throw new Error(result.error || 'Something went wrong');
    return result;
    
  } catch (error: any) {
    throw new Error(error.message || 'Network error');
  }
};
