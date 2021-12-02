#include "d3d12-backend.h"

#include "../../RHI/misc.h"

namespace redtea
{
namespace device
{
	TimerQuery::~TimerQuery()
	{
		m_Resources.timerQueries.release(static_cast<int>(beginQueryIndex) / 2);
	}

	EventQueryHandle Device::createEventQuery(void)
	{
		EventQuery *ret = new EventQuery();
		return EventQueryHandle::Create(ret);
	}

	void Device::setEventQuery(IEventQuery* _query, CommandQueue queue)
	{
		EventQuery* query = CHECKED_CAST<EventQuery*>(_query);
		Queue* pQueue = getQueue(queue);

		query->started = true;
		query->fence = pQueue->fence;
		query->fenceCounter = pQueue->lastSubmittedInstance;
		query->resolved = false;
	}

	bool Device::pollEventQuery(IEventQuery* _query)
	{
		EventQuery* query = CHECKED_CAST<EventQuery*>(_query);

		if (!query->started)
			return false;

		if (query->resolved)
			return true;

		assert(query->fence);

		if (query->fence->GetCompletedValue() >= query->fenceCounter)
		{
			query->resolved = true;
			query->fence = nullptr;
		}

		return query->resolved;
	}

	void Device::waitEventQuery(IEventQuery* _query)
	{
		EventQuery* query = CHECKED_CAST<EventQuery*>(_query);

		if (!query->started || query->resolved)
			return;

		assert(query->fence);

		WaitForFence(query->fence, query->fenceCounter, m_FenceEvent);
	}

	void Device::resetEventQuery(IEventQuery* _query)
	{
		EventQuery* query = CHECKED_CAST<EventQuery*>(_query);

		query->started = false;
		query->resolved = false;
		query->fence = nullptr;
	}

	TimerQueryHandle Device::createTimerQuery(void)
	{
		int queryIndex = m_Resources.timerQueries.allocate();

		if (queryIndex < 0)
			return nullptr;

		TimerQuery* query = new TimerQuery(m_Resources);
		query->beginQueryIndex = uint32_t(queryIndex) * 2;
		query->endQueryIndex = query->beginQueryIndex + 1;
		query->resolved = false;
		query->time = 0.f;

		return TimerQueryHandle::Create(query);
	}

	bool Device::pollTimerQuery(ITimerQuery* _query)
	{
		TimerQuery* query = CHECKED_CAST<TimerQuery*>(_query);

		if (!query->started)
			return false;

		if (!query->fence)
			return true;

		if (query->fence->GetCompletedValue() >= query->fenceCounter)
		{
			query->fence = nullptr;
			return true;
		}

		return false;
	}

	float Device::getTimerQueryTime(ITimerQuery* _query)
	{
		TimerQuery* query = CHECKED_CAST<TimerQuery*>(_query);

		if (!query->resolved)
		{
			if (query->fence)
			{
				WaitForFence(query->fence, query->fenceCounter, m_FenceEvent);
				query->fence = nullptr;
			}

			uint64_t frequency;
			getQueue(CommandQueue::Graphics)->queue->GetTimestampFrequency(&frequency);

			D3D12_RANGE bufferReadRange = {
				query->beginQueryIndex * sizeof(uint64_t),
				(query->beginQueryIndex + 2) * sizeof(uint64_t) };
			uint64_t *data;
			const HRESULT res = m_Context.timerQueryResolveBuffer->resource->Map(0, &bufferReadRange, (void**)&data);

			if (FAILED(res))
			{
				m_Context.error("getTimerQueryTime: Map() failed");
				return 0.f;
			}

			query->resolved = true;
			query->time = float(double(data[query->endQueryIndex] - data[query->beginQueryIndex]) / double(frequency));

			m_Context.timerQueryResolveBuffer->resource->Unmap(0, nullptr);
		}

		return query->time;
	}

	void Device::resetTimerQuery(ITimerQuery* _query)
	{
		TimerQuery* query = CHECKED_CAST<TimerQuery*>(_query);

		query->started = false;
		query->resolved = false;
		query->time = 0.f;
		query->fence = nullptr;
	}

	void CommandList::beginTimerQuery(ITimerQuery* _query)
	{
		TimerQuery* query = CHECKED_CAST<TimerQuery*>(_query);

		m_Instance->referencedTimerQueries.push_back(query);

		m_ActiveCommandList->commandList->EndQuery(m_Context.timerQueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, query->beginQueryIndex);

		// two timestamps within the same command list are always reliably comparable, so we avoid kicking off here
		// (note: we don't call SetStablePowerState anymore)
	}

	void CommandList::endTimerQuery(ITimerQuery* _query)
	{
		TimerQuery* query = CHECKED_CAST<TimerQuery*>(_query);

		m_Instance->referencedTimerQueries.push_back(query);

		m_ActiveCommandList->commandList->EndQuery(m_Context.timerQueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, query->endQueryIndex);

		m_ActiveCommandList->commandList->ResolveQueryData(m_Context.timerQueryHeap,
			D3D12_QUERY_TYPE_TIMESTAMP,
			query->beginQueryIndex,
			2,
			m_Context.timerQueryResolveBuffer->resource,
			query->beginQueryIndex * 8);
	}


}
}