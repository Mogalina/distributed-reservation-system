'use client';

import React, { useEffect, useState, use } from 'react';
import { useRouter } from 'next/navigation';
import { useAuth } from '../../../hooks/useAuth';
import { fetchEventDetails, reserveTickets } from '../../../services/api';
import { EventDetails } from '../../../types';
import Card from '../../../components/Card';
import Button from '../../../components/Button';

export default function EventPage({ params }: { params: Promise<{ id: string }> }) {
  const { id } = use(params);
  const { user, loading } = useAuth();
  const router = useRouter();

  const [event, setEvent] = useState<EventDetails | null>(null);
  const [ticketCount, setTicketCount] = useState(1);
  const [selectedCat, setSelectedCat] = useState<string>('');
  const [msg, setMsg] = useState('');
  const [error, setError] = useState('');

  useEffect(() => {
    if (!loading && !user) {
      router.push('/auth');
    }
    
    if (user && id) {
      fetchEventDetails(user.token, id)
        .then(setEvent)
        .catch(err => setError(err.message));
    }
  }, [user, loading, id, router]);

  const handleReserve = async () => {
    if (!selectedCat) {
      setError("Please select a category");
      return;
    }
    
    try {
      setError('');
      setMsg('');
      await reserveTickets(user!.token, {
        eventId: id,
        categoryId: selectedCat,
        ticketCount: ticketCount
      });
      setMsg("Reservation Successful!");
      
      // Refresh data to update availability
      fetchEventDetails(user!.token, id).then(setEvent);
      
    } catch (err: any) {
      setError(err.message || "Reservation failed");
    }
  };

  if (loading || !event) return <div className="p-10 text-center">Loading...</div>;

  return (
    <div className="container" style={{ maxWidth: '800px', padding: '0 20px' }}>
      <Card>
        <div style={{ position: 'relative' }}>
            <button 
              onClick={() => router.push('/')}
              style={{
                position: 'absolute',
                top: '-10px', 
                right: '-10px',
                width: '32px',
                height: '32px',
                borderRadius: '50%',
                background: '#f5f5f7',
                border: 'none',
                display: 'flex',
                alignItems: 'center',
                justifyContent: 'center',
                cursor: 'pointer',
                color: '#666',
                fontSize: '16px',
                transition: 'all 0.2s',
                zIndex: 10
              }}
              onMouseOver={(e) => e.currentTarget.style.background = '#e5e5ea'}
              onMouseOut={(e) => e.currentTarget.style.background = '#f5f5f7'}
              title="Back to Dashboard"
            >
              ✕
            </button>

            <h1 style={{ fontSize: '32px', marginBottom: '10px', paddingRight: '40px' }}>{event.eventName}</h1>
            <p style={{ color: '#86868b', marginBottom: '30px' }}>
              {event.startTime} - {event.endTime}
            </p>
        </div>

        {msg && <div style={{ color: 'green', marginBottom: '20px' }}>{msg}</div>}
        {error && <div style={{ color: 'red', marginBottom: '20px' }}>{error}</div>}

        <h3>Select Tickets</h3>
        <div style={{ display: 'grid', gap: '15px', marginTop: '15px' }}>
          {event.categories.map(cat => (
            <div 
              key={cat.categoryId}
              onClick={() => setSelectedCat(cat.categoryId)}
              style={{
                border: selectedCat === cat.categoryId ? '2px solid #000' : '1px solid #eaeaea',
                padding: '15px',
                borderRadius: '12px',
                cursor: 'pointer',
                display: 'flex',
                justifyContent: 'space-between',
                opacity: cat.available === 0 ? 0.5 : 1,
                pointerEvents: cat.available === 0 ? 'none' : 'auto',
                transition: 'border-color 0.2s'
              }}
            >
              <div>
                <strong>{cat.name}</strong>
                <div style={{ fontSize: '14px', color: '#666' }}>${cat.price}</div>
              </div>
              <div style={{ textAlign: 'right' }}>
                <div style={{ fontWeight: 'bold' }}>{cat.available} left</div>
                {cat.available === 0 && <span style={{ color: 'red', fontSize: '12px' }}>SOLD OUT</span>}
              </div>
            </div>
          ))}
        </div>

        <div style={{ marginTop: '30px', display: 'flex', gap: '10px', alignItems: 'center' }}>
          <label style={{ fontWeight: 500 }}>Quantity:</label>
          <input 
            type="number" 
            min="1" 
            max="10" 
            value={ticketCount} 
            onChange={(e) => setTicketCount(parseInt(e.target.value))}
            style={{ 
                padding: '10px', 
                borderRadius: '8px', 
                border: '1px solid #d2d2d7', 
                width: '80px',
                fontSize: '16px',
                outline: 'none'
            }}
          />
          <Button onClick={handleReserve} style={{ width: 'auto', padding: '10px 30px', marginBottom: '0' }}>
            Reserve Now
          </Button>
        </div>
      </Card>
    </div>
  );
}
