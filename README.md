# GBEmu

GBEmu é um emulador de Game Boy escrito em C. Ele implementa os principais componentes de um emulador, incluindo uma CPU, MMU e agora a futura implementação de uma PPU (Picture Processing Unit).

## Componentes

### CPU (Central Processing Unit)
A CPU é responsável pela execução de instruções da ROM e pelo controle geral do emulador.

### MMU (Memory Management Unit)
A MMU gerencia o mapeamento de memória, acessos à ROM, RAM e IO.

### PPU (Picture Processing Unit)
A PPU é responsável pelo processamento gráfico do emulador. Ela controla a renderização de tiles, sprites e camadas do Game Boy por meio dos modos de renderização: HBlank, VBlank, OAM Search e Drawing.

**Funcionalidades adicionadas:**
- Implementação dos registradores principais: LCDC, SCY, SCX, LY.
- Ciclo básico da PPU: varredura das linhas.
- Integração inicial com a MMU e ciclo principal.

**Funcionalidades futuras da PPU:**
- Implementar registradores gráficos (LCDC, SCX, SCY, LY, etc.).
- Ciclo de renderização com os modos HBlank, VBlank, OAM Search e Drawing.
- Renderização de tiles e paletas.
- Manipulação de sprites e priorização de camadas.

## Como executar

1. Certifique-se de ter o SDL2 configurado.
2. Compile o projeto utilizando CMake:
   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```
3. Execute o emulador:
   ```bash
   ./gbemu <nome_da_rom.gb>
   ```

## Requisitos
- SDL2 Development Library
- Compilador C (gcc/clang)

## Licença
Este projeto é desenvolvido para fins educacionais e sem fins lucrativos.