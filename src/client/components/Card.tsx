import React from 'react';

interface CardProps {
  children: React.ReactNode;
  className?: string;
}

const styles: React.CSSProperties = {
  background: 'var(--card-bg)',
  padding: '48px 40px', 
  borderRadius: '24px',
  boxShadow: '0 20px 60px rgba(0,0,0,0.12)',
  width: '100%',
};

const Card: React.FC<CardProps> = ({ children, className }) => (
  <div style={styles} className={className}>
    {children}
  </div>
);

export default Card;