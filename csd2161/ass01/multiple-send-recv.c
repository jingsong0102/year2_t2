
bool sendBase(int s, const void *data, int sz)
{
	const char *buffer = (const char*)data;
	while (sz > 0)
	{
		int sent = send(s, buffer, sz, 0);
		if (sent < 0) {
			if (sent == SOCKET_ERROR) {
#ifdef DEBUG
				printf("send failed: %d\n", WSAGetLastError());
#endif
			}

			return false;
		}
		buffer += sent;
		sz -= sent;
	}

	return true;
}

int readBase(int s, void *data, int sz)
{
	char *buffer = (char*)data;
	while (sz > 0)
	{
		int rcvd = recv(s, buffer, sz, 0);
		if (rcvd < 0) {
#ifdef DEBUG
			printf("recv failed: %d\n", WSAGetLastError());
#endif
			return -1;
		}
		if (rcvd == 0)
			return 0;
		buffer += rcvd;
		sz -= rcvd;
	}
	return 1;
}