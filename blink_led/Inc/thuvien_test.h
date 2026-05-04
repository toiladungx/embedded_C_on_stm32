typedef struct
{
   uint32_t CR;            /*!< TODO,     										Address offset: 0x00 */
   uint32_t PLLCFGR;       /*!< TODO,     										Address offset: 0x04 */
   uint32_t CFGR;          /*!< TODO,     										Address offset: 0x08 */
   uint32_t CIR;           /*!< TODO,     										Address offset: 0x0C */
   uint32_t AHB1RSTR;      /*!< TODO,     										Address offset: 0x10 */
   uint32_t AHB2RSTR;      /*!< TODO,     										Address offset: 0x14 */
   uint32_t AHB3RSTR;      /*!< TODO,     										Address offset: 0x18 */
  uint32_t      RESERVED0;     /*!< Reserved, 0x1C                                                       */
   uint32_t APB1RSTR;      /*!< TODO,     										Address offset: 0x20 */
   uint32_t APB2RSTR;      /*!< TODO,     										Address offset: 0x24 */
  uint32_t      RESERVED1[2];  /*!< Reserved, 0x28-0x2C                                                  */
   uint32_t AHB1ENR;       /*!< TODO,     										Address offset: 0x30 */
   uint32_t AHB2ENR;       /*!< TODO,     										Address offset: 0x34 */
   uint32_t AHB3ENR;       /*!< TODO,     										Address offset: 0x38 */
  	  uint32_t      RESERVED2;     /*!< Reserved, 0x3C                                                       */
   uint32_t APB1ENR;       /*!< TODO,     										Address offset: 0x40 */
   uint32_t APB2ENR;       /*!< TODO,     										Address offset: 0x44 */
  uint32_t      RESERVED3[2];  /*!< Reserved, 0x48-0x4C                                                  */
   uint32_t AHB1LPENR;     /*!< TODO,     										Address offset: 0x50 */
   uint32_t AHB2LPENR;     /*!< TODO,     										Address offset: 0x54 */
   uint32_t AHB3LPENR;     /*!< TODO,     										Address offset: 0x58 */
  uint32_t      RESERVED4;     /*!< Reserved, 0x5C                                                       */
   uint32_t APB1LPENR;     /*!< TODO,     										Address offset: 0x60 */
   uint32_t APB2LPENR;     /*!< RTODO,     										Address offset: 0x64 */
  uint32_t      RESERVED5[2];  /*!< Reserved, 0x68-0x6C                                                  */
   uint32_t BDCR;          /*!< TODO,     										Address offset: 0x70 */
   uint32_t CSR;           /*!< TODO,     										Address offset: 0x74 */
  uint32_t      RESERVED6[2];  /*!< Reserved, 0x78-0x7C                                                  */
   uint32_t SSCGR;         /*!< TODO,     										Address offset: 0x80 */
   uint32_t PLLI2SCFGR;    /*!< TODO,     										Address offset: 0x84 */
   uint32_t PLLSAICFGR;    /*!< TODO,     										Address offset: 0x88 */
   uint32_t DCKCFGR;       /*!< TODO,     										Address offset: 0x8C */
   uint32_t CKGATENR;      /*!< TODO,     										Address offset: 0x90 */
   uint32_t DCKCFGR2;      /*!< TODO,     										Address offset: 0x94 */

} RCC_RegDef_t;


#define GPIOA_BASEADDR  					0x40010800U
#define RCC_BASEADDR                     	0x40021000U


// Offset thanh ghi
#define RCC_APB2ENR_OFFSET  0x18U
#define GPIO_CRH_OFFSET     0x04U
#define GPIO_ODR_OFFSET     0x0CU
//#define RCC 								((RCC_RegDef_t*)RCC_BASEADDR)

void SetClockForGPIOA()
{
	//(RCC->AHB1ENR |= (1 << 0))
	uint32_t *pointer_to_reg = (uint32_t *)(RCC_BASEADDR + RCC_APB2ENR_OFFSET); // Point to 0x400210018 (RCC_APB2ENR register address)
	*pointer_to_reg |= 0xFFFFFFFF;
}
