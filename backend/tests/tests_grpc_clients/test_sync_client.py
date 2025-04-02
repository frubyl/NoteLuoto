import pytest
from unittest.mock import AsyncMock, MagicMock
import grpc
from samples.SyncService_pb2 import (
    NoteRequest,
    DeleteRequest,
    EmptyResponse
)
from samples.SyncService_pb2_grpc import SyncServiceStub

@pytest.fixture
def mock_sync_client():
    """Фикстура для мока gRPC клиента SyncService"""
    client = MagicMock(spec=SyncServiceStub)
    
    # Настраиваем моки для всех методов
    client.AddNote = AsyncMock(return_value=EmptyResponse())
    client.UpdateNote = AsyncMock(return_value=EmptyResponse())
    client.DeleteNote = AsyncMock(return_value=EmptyResponse())
    
    return client

async def test_grpc_sync_client_add_note_success(mock_sync_client):
    """Тестирование успешного добавления заметки через gRPC SyncService клиент"""
    request = NoteRequest(note_id=123, full_text="Sample note text")
    response = await mock_sync_client.AddNote(request)
    
    assert isinstance(response, EmptyResponse)
    mock_sync_client.AddNote.assert_called_once_with(request)

async def test_grpc_sync_client_add_note_error(mock_sync_client):
    """Тестирование обработки ошибки при добавлении заметки через gRPC SyncService клиент"""
    mock_sync_client.AddNote.side_effect = grpc.RpcError("AddNote failed")
    
    with pytest.raises(grpc.RpcError):
        await mock_sync_client.AddNote(NoteRequest(note_id=123, full_text="Error"))
    
    mock_sync_client.AddNote.assert_called_once()

async def test_grpc_sync_client_update_note_success(mock_sync_client):
    """Тестирование успешного обновления заметки через gRPC SyncService клиент"""
    request = NoteRequest(note_id=456, full_text="Updated text")
    response = await mock_sync_client.UpdateNote(request)
    
    assert isinstance(response, EmptyResponse)
    mock_sync_client.UpdateNote.assert_called_once_with(request)


async def test_grpc_sync_client_delete_note_success(mock_sync_client):
    """Тестирование успешного удаления заметки через gRPC SyncService клиент"""
    request = DeleteRequest(note_id=789)
    response = await mock_sync_client.DeleteNote(request)
    
    assert isinstance(response, EmptyResponse)
    mock_sync_client.DeleteNote.assert_called_once_with(request)


async def test_grpc_sync_client_concurrent_updates(mock_sync_client):
    """Тестирование последовательных обновлений через gRPC SyncService клиент"""
    request1 = NoteRequest(note_id=111, full_text="First update")
    request2 = NoteRequest(note_id=111, full_text="Second update")
    
    await mock_sync_client.UpdateNote(request1)
    await mock_sync_client.UpdateNote(request2)
    
    assert mock_sync_client.UpdateNote.call_count == 2
    calls = mock_sync_client.UpdateNote.call_args_list
    assert calls[0][0][0].full_text == "First update"
    assert calls[1][0][0].full_text == "Second update"