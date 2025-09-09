import os

API_URL = os.getenv("API_URL", "http://localhost:8080")
REDIS_HOST = os.getenv("REDIS_HOST", "redis")
REDIS_PORT = int(os.getenv("REDIS_PORT", 6379))
