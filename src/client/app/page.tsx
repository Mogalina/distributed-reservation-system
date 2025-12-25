'use client';

import React, { useEffect } from 'react';
import { useRouter } from 'next/navigation';
import { useAuth } from '../hooks/useAuth';
import Card from '../components/Card';
import Button from '../components/Button';

export default function HomePage() {
  const { user, logout, loading } = useAuth();
  const router = useRouter();

  useEffect(() => {
    if (!loading && !user) {
      router.push('/auth');
    }
  }, [user, loading, router]);

  if (loading || !user) return null;

  return (
    <div className="container text-center">
      <Card>
        <p style={{ textTransform: 'uppercase', letterSpacing: '2px', fontSize: '12px', marginBottom: '10px' }}>Dashboard</p>
        <h1 style={{ fontSize: '32px', marginBottom: '10px' }}>Hello, {user?.userId}</h1>
        <p className="mb-4">You are successfully logged in.</p>
        
        <Button 
          onClick={() => {
            logout();
            router.push('/auth');
          }}
          style={{ background: 'transparent', border: '1px solid #d2d2d7', color: 'var(--text-main)', marginTop: '30px' }}
        >
          Sign Out
        </Button>
      </Card>
    </div>
  );
}
