'use client';

import React, { useState, FormEvent, ChangeEvent } from 'react';
import { useRouter } from 'next/navigation';
import { useAuth } from '../../hooks/useAuth';
import { authRequest } from '../../services/api';
import Card from '../../components/Card';
import Input from '../../components/Input';
import Button from '../../components/Button';

export default function AuthPage() {
  const { login } = useAuth();
  const router = useRouter();
  
  const [isRegistering, setIsRegistering] = useState(false);
  const [error, setError] = useState('');
  const [loading, setLoading] = useState(false);
  
  const [formData, setFormData] = useState({
    userId: '',
    nationalId: '',
    firstName: '',
    lastName: ''
  });

  const handleChange = (e: ChangeEvent<HTMLInputElement>) => {
    setFormData({ ...formData, [e.target.name]: e.target.value });
    setError('');
  };

  const handleSubmit = async (e: FormEvent) => {
    e.preventDefault();
    setLoading(true);
    try {
      const endpoint = isRegistering ? 'register' : 'login';
      const res = await authRequest(endpoint, formData);
      
      login({ userId: res.userId, token: res.token });
      router.push('/');
    } catch (err: any) {
      setError(err.message);
    } finally {
      setLoading(false);
    }
  };

  return (
    <div className="container">
      <Card>
        <div className="text-center mb-4">
          <div style={{ width: '40px', height: '40px', background: '#000', borderRadius: '50%', margin: '0 auto 15px' }}></div>
          <h1>{isRegistering ? 'Create Account' : 'Welcome Back'}</h1>
          <p>{isRegistering ? 'Join the reservation system.' : 'Please enter your details.'}</p>
        </div>

        {error && <div className="error-msg">{error}</div>}

        <form onSubmit={handleSubmit}>
          <Input 
            label="User ID"
            name="userId" 
            placeholder="username" 
            value={formData.userId} 
            onChange={handleChange} 
            required 
          />

          {isRegistering && (
            <>
              <Input 
                label="First Name"
                name="firstName" 
                placeholder="John" 
                value={formData.firstName} 
                onChange={handleChange} 
                required 
              />
              <Input 
                label="Last Name"
                name="lastName" 
                placeholder="Doe" 
                value={formData.lastName} 
                onChange={handleChange} 
                required 
              />
            </>
          )}

          <Input 
            label="National ID"
            name="nationalId" 
            placeholder="Your unique ID" 
            value={formData.nationalId} 
            onChange={handleChange} 
            required 
          />

          <Button disabled={loading}>
            {loading ? 'Processing...' : (isRegistering ? 'Sign Up' : 'Sign In')}
          </Button>
        </form>

        <button className="link-btn" onClick={() => setIsRegistering(!isRegistering)}>
          {isRegistering ? 'Already have an account? Sign In' : 'New here? Create Account'}
        </button>
      </Card>
    </div>
  );
}
