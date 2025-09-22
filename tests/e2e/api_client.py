import requests
from tests.config import ALLOCATION_URL


def post_to_add_batch(ref, sku, qty, eta):
    r = requests.post(
        f"{ALLOCATION_URL}/add_batch", json={"ref": ref, "sku": sku, "qty": qty, "eta": eta}
    )
    assert r.status_code == 201


def post_to_allocate(orderid, sku, qty, expect_success=True):
    r = requests.post(
        f"{ALLOCATION_URL}/allocate",
        json={
            "orderid": orderid,
            "sku": sku,
            "qty": qty,
        },
    )
    if expect_success:
        assert r.status_code == 202
    return r


def get_allocation(orderid):
    return requests.get(f"{ALLOCATION_URL}/allocations/{orderid}")
