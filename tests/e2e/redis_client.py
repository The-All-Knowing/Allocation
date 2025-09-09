import json
import redis

from ..config import REDIS_HOST, REDIS_PORT

r = redis.Redis(REDIS_HOST, REDIS_PORT)


def subscribe_to(channel):
    pubsub = r.pubsub()
    pubsub.subscribe(channel)
    confirmation = pubsub.get_message(timeout=3)
    assert confirmation["type"] == "subscribe"
    return pubsub


def publish_message(channel, message):
    r.publish(channel, json.dumps(message))