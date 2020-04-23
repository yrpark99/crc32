#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#define FILE_READ_BUF_SIZE			(512 * 1024)

#define CRC32_NORMAL_POLYNOMIAL		0xEDB88320
#define CRC32_NORMAL_INITIAL_VALUE	0xFFFFFFFF
#define CRC32_NORMAL_XOR_OUT_VALUE	0xFFFFFFFF

#define CRC32_MPEG2_POLYNOMIAL		0x04C11DB7
#define CRC32_MPEG2_INITIAL_VALUE	0xFFFFFFFF
#define CRC32_MPEG2_XOR_OUT_VALUE	0x0

typedef enum
{
	CRC32_NORMAL = 1,
	CRC32_MPEG2 = 2
} CRC32_TYPE_E;

typedef struct
{
	uint32_t table[256];
	uint32_t value;
} crc32_t;

void crc32_init(CRC32_TYPE_E type, crc32_t *crc)
{
	uint32_t v;

	if (type == CRC32_NORMAL)
	{
		for (int i = 0; i < 256; i++)
		{
			v = i;
			for (int j = 0; j < 8; j++)
			{
				v = (v & 1) ? ((v >> 1) ^ CRC32_NORMAL_POLYNOMIAL) : (v >> 1);
			}
			crc->table[i] = v;
		}
	}
	else if (type == CRC32_MPEG2)
	{
		for (int i = 0; i < 256; i++)
		{
			v = i << 24;
			for (int j = 0; j < 8; j++)
			{
				v = (v & 0x80000000) ? ((v << 1) ^ CRC32_MPEG2_POLYNOMIAL) : (v << 1);
			}
			crc->table[i] = v;
		}
	}
}

void crc32_update(CRC32_TYPE_E type, crc32_t *c, uint8_t *buf, size_t len)
{
	if (type == CRC32_NORMAL)
	{
		for (size_t i = 0; i < len; i++)
		{
			c->value = c->table[(c->value ^ buf[i]) & 0xFF] ^ (c->value >> 8);
		}
	}
	else if (type == CRC32_MPEG2)
	{
		for (size_t i = 0; i < len; i++)
		{
			c->value = c->table[((c->value >> 24) ^ buf[i]) & 0xFF] ^ (c->value << 8);
		}
	}
}

void crc32_start(CRC32_TYPE_E type, crc32_t *c)
{
	if (type == CRC32_NORMAL)
	{
		c->value = CRC32_NORMAL_INITIAL_VALUE;
	}
	else if (type == CRC32_MPEG2)
	{
		c->value = CRC32_MPEG2_INITIAL_VALUE;
	}
}

uint32_t crc32_finalize(CRC32_TYPE_E type, crc32_t *c)
{
	if (type == CRC32_NORMAL)
	{
		return c->value ^ CRC32_NORMAL_XOR_OUT_VALUE;
	}
	else if (type == CRC32_MPEG2)
	{
		return c->value ^ CRC32_MPEG2_XOR_OUT_VALUE;
	}
	return c->value;
}

uint32_t crc32_crc(CRC32_TYPE_E type, crc32_t *c, uint8_t *buf, size_t len)
{
	crc32_start(type, c);
	crc32_update(type, c, buf, len);
	return crc32_finalize(type, c);
}

void show_help(void)
{
	printf("Function: Calculate the CRC32, CRC32_MPEG2 of a given file\n");
	printf("Option: <file_name>\n");
}

int main(int argc, char *argv[])
{
	crc32_t c1, c2;
	size_t buf_size = FILE_READ_BUF_SIZE;
	size_t len;

	if (argc < 2)
	{
		show_help();
		return 1;
	}

	FILE *fp = fopen(argv[1], "rb");
	if (fp == NULL)
	{
		printf("Failed to open '%s' file", argv[1]);
		return 1;
	}

	uint8_t *buf = malloc(buf_size);
	if (buf == NULL)
	{
		printf("Failed to allocate memory");
		fclose(fp);
		return 1;
	}

	crc32_init(CRC32_NORMAL, &c1);
	crc32_init(CRC32_MPEG2, &c2);
	crc32_start(CRC32_NORMAL, &c1);
	crc32_start(CRC32_MPEG2, &c2);

	while ((len = fread(buf, 1, buf_size, fp)) > 0)
	{
		crc32_update(CRC32_NORMAL, &c1, buf, len);
		crc32_update(CRC32_MPEG2, &c2, buf, len);
	}
	printf("CRC32: 0x%08X\n", crc32_finalize(CRC32_NORMAL, &c1));
	printf("CRC32_MPEG2: 0x%08X\n", crc32_finalize(CRC32_MPEG2, &c2));

	free(buf);
	fclose(fp);
	return 0;
}
