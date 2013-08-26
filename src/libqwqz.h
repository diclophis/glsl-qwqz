// 

struct qwqz_handle_t {
    //const yajl_callbacks * callbacks;
    void * ctx;
    //yajl_lexer lexer;
    //const char * parseError;
    /* the number of bytes consumed from the last client buffer,
     * in the case of an error this will be an error offset, in the
     * case of an error this can be used as the error offset */
    //size_t bytesConsumed;
    /* temporary storage for decoded strings */
    //yajl_buf decodeBuf;
    /* a stack of states.  access with yajl_state_XXX routines */
    //yajl_bytestack stateStack;
    /* memory allocation routines */
    //yajl_alloc_funcs alloc;
    /* bitfield */
    //unsigned int flags;
    int m_SpriteCount;
    int m_IsSceneBuilt;
    int m_IsScreenResized;
    float m_SimulationTime;
    int m_GameState;
    int m_Program;
    int m_EnabledState;
    float m_ScreenWidth;
    float m_ScreenHeight;
    float m_ScreenAspect;
    float m_ScreenHalfHeight;
};

typedef struct qwqz_handle_t * qwqz_handle;

qwqz_handle qwqz_alloc();

int qwqz_init(qwqz_handle e);
char *qwqz_load(const char *path);
int qwqz_shader();

int qwqz_link(qwqz_handle e);
int qwqz_draw(qwqz_handle e);
int qwqz_resize(qwqz_handle e, float width, float height);
