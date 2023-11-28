// allocate memory on stack
#define salloc(size) (void *)(char [size]){0}

typedef bool cmp_fn(void *x, void *y);

struct vec {
	int len, cap, size;
	char buf[];
};

static struct vec *vec_init(struct vec *v, int cap, int size)
{
	v->len = 0;
	v->cap = cap;
	v->size = size;
	return v;
}

#define vec_new(cap, size) \
	vec_init(salloc(sizeof(struct vec) + (cap) * (size)), (cap), (size))

static void vec_push(struct vec *v, void *elem)
{
	assert(v->len < v->cap);
	memcpy(v->buf + v->len * v->size, elem, v->size);
	++v->len;
}

static void *vec_at(struct vec *v, int i)
{
	assert(i < v->len);
	return v->buf + i * v->size;
}

static bool vec_contains(struct vec *v, void *elem)
{
	for (int i = 0; i < v->len; ++i)
		if (0 == memcpy(vec_at(v, i), elem, v->size))
			return true;
	return false;
}

static bool vec_contains_cmp(struct vec *v, void *elem, cmp_fn cmp)
{
	for (int i = 0; i < v->len; ++i)
		if (cmp(vec_at(v, i), elem))
			return true;
	return false;
}

struct queue {
	int front, back, cap, size;
	char buf[];
};

static struct queue *queue_init(struct queue *q, int cap, int size)
{
	q->front = 0;
	q->back = 0;
	q->cap = cap;
	q->size = size;
	return q;
}

#define queue_new(cap, size) \
	queue_init(salloc(sizeof(struct queue) + (cap) * (size)), (cap), (size))

static void queue_push(struct queue *q, void *elem)
{
	assert(q->back < q->cap);
	memcpy(q->buf + q->back * q->size, elem, q->size);
	++q->back;
}

static void queue_pop(struct queue *q)
{
	assert(q->front < q->back);
	++q->front;
}

static void *queue_front(struct queue *q)
{
	return q->buf + q->front * q->size;
}

static bool queue_empty(struct queue *q)
{
	return q->front == q->back;
}

typedef unsigned *bit_set_t;

#define bit_set_new(bits) (unsigned [(bits) / 32]){0}

static bool bit_set_contains(bit_set_t bs, unsigned i)
{
	return bs[i / 32] & (1 << i % 32);
}

static void bit_set_add(bit_set_t bs, unsigned i)
{
	bs[i / 32] |= (1 << i % 32);
}
