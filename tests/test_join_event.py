import pytest

from testsuite.databases import pgsql

# Запустите тесты с помощью pytest: pytest test_join_event.py --asyncio-mode=auto
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_join_event(service_client):
    
    # Шаг 2: Пользователь пытается присоединиться к событию
    join_response = await service_client.post(
        'join_event',
        headers = {'X-Ya-User-Ticket': 'asdf'},
        json = {'event_id': 2}
    )
    assert join_response.status == 200
    join_data = join_response.json()
    assert join_data == 'success'
