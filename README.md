# Messager-Elephango
Мессенджер на C++ с графическим интерфейсом

# Здесь должно быть ТЗ, но оно пока в docs

Создание Тестовой БД:

### 
    CREATE TABLE messages (
    id SERIAL PRIMARY KEY,
    sender VARCHAR(255),
    message TEXT,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
    );
###