import os

ALLOCATION_URL = "http://localhost:" + os.getenv("ALLOCATION_PORT", "8080")
REDIS_HOST = os.getenv("REDIS_HOST", "localhost")
REDIS_PORT = int(os.getenv("REDIS_PORT", 6379))
