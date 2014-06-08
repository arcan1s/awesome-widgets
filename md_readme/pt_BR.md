pytextmonitor
=============

Informação
-----------

PyTextMonitor é um script Plasmoid escrito em Python2. Assemelha-se com widgets no awesome-wm.

**NOTA** [PROCURA_SE TRADUTORES!](https://github.com/arcan1s/pytextmonitou/issues/14)

Configuração
------------

Para editar o output, você deve abrir a janela de Configurações e configurar o formato de output na linhas:

* tag `$time` - hora no formato padrão. Por exemplo, *sex Nov 6 04:48:01 2013*
* tag `$isotime` - hora no formato ISO.
* tag `$shouttime` - hora em formato curto
* tag `$longtime` - hora em formato longo
* tag `$custom` - formato personalizado de houa
* tag `$uptime` - tempo em atividade, *---d--h--m*
* tag `$custom` - formato personalizado de tempo em atividade
* tag `$cpu` - carga total da CPU, *%*
* tag `$cpuN` - carga total da CPU no núcleo N, *%*
* tag `$cpucl` - frequência média da CPU, *MHz*
* tag `$cpuclN` - frequência do núcleo N da CPU, *MHz*
* tag `$tempN` - temperatura do dispositivo N. Por exemplo, `$temp0`
* tag `$gpu` - uso da GPU, *%*. `aticonfig` ou `nvidia-smi` devem estar instalados
* tag `$gputemp` - temperatura da GPU. `aticonfig` ou `nvidia-smi` devem estar instalados
* tag `$mem` - uso de memória, *%*
* tag `$memmb` - uso de memória, *MB*
* tag `$memgb` - uso de memória, *GB*
* tag `$memtotmb` - RAM total, *MB*
* tag `$memtotgb` - RAM total, *GB*
* tag `$swap` - swap, *%*
* tag `$swapmb` - swap, *MB*
* tag `$swapgb` - swap, *GB*
* tag `$swaptotmb` - swap total, *MB*
* tag `$swaptotgb` - swap total, *GB*
* tag `$hddN` - uso do ponto de montagem N, *%*. Pou exemplo, `$hdd0`
* tag `$hddmbN` - uso do ponto de montagem, *MB*. Por exemplo, `$hddmb0`
* tag `$hddgbN` - uso do ponto de montagem, *GB*. Por exemplo, `$hddgb0`
* tag `$hddtotmbN` - tamanho total do ponto de montagem N, *MB*. Por exemplo, `$hddtotmb0`
* tag `$hddtotgbN` - tamanho total do ponto de montagem N,, *GB*. Por exemplo, `$hddtotgb0`
* tag `$hddrN` - velocidade de leitura do disco N, *KB/s*. Por exemplo, `$hddr0`
* tag `$hddwN` - velocidade de escrita do disco N, *KB/s*. Por exemplo, `$hddw0`
* tag `$hddtempN` - temperatura do HDD N. Por exemplo, `$hddtemp0`
* tag `$down` - velocidade de download, *KB/s*
* tag `$up` - velocidade de upload, *KB/s*
* tag `$netdev` - dispositivo atual de rede
* tag `$bat` - carga da bateria, *%*
* tag `$ac` - status do dispositivo carregador. Retorna (\*) se o carregador estiver plugado ou *( )* se estiver desplugado
* tag `$album` - álbum da música atual . Um dos reprodutores de músicas suportados deve estar instalado
* tag `$artist` - artista da música atual. Um dos reprodutores de músicas suportados deve estar instalado
* tag `$progress` - progresso da música atual. Um dos reprodutores de músicas suportados deve estar instalado
* tag `$time` - tempo da música atual. Um dos reprodutores de músicas suportados deve estar instalado
* tag `$title` - título da música atual. Um dos reprodutores de músicas suportados deve estar instalado
* tag `$pscount` - número de processos rodando
* tag `$pstotal` - número total de processos
* tag `$ps` - lista de processos rodando
* tag `$pkgcountN` - número de pacotes disponíveis para atualizar pelo comando N available to upgrade. Por exemplo, `$pkgcount0`
* tag `$customN` - retorna o output do comando N. Por exemplo, `$custom0`

A ordem dos rótulos mudará se você mudar a posição do slider. Tags HTML funcionarão normalmente.

**NOTA** você não deve usar a tag `$cpu` no rótulo de swap, por exemplo. **`$cpu` somente funcionará no rótulo da cpu**.

Dicas & truques
---------------

Você pode usar cores diferentes para os rótulos. Só coloque o texto do rótulo em código HTML. Veja este [issue](https://github.com/arcan1s/pytextmonitor/issues/9) para mais detalhes.

A numeração dos elementos de temperatura, uso de HDD, velocidade do HDD, temperatura do HDD refere à ordem dos elementos da segunda aba (*Configurações Avançadas*). Você deve adicionar o item para a lista requerida listWidget primeiro. E o primeiro elemento na listWidget será `$tag0`. Veja este [issue](https://github.com/arcan1s/pytextmonitor/issues/17) para mais detalhes.

Configurações Avançadas
-----------------------

**Layout vertical**

Use o layout vertical ao invés do horizontal.

**Habilitar popup**

Desmarque esta caixa se não quiser usar mensagens popup

**Adicionar esticamento**

Adicionar esticamento (espaço) para o lado selecionado do widget.

**Hora customizada**

* tag `$dddd` - dia da semana em formato completo
* tag `$ddd` - dia da semana em format curto
* tag `$dd` - dia
* tag `$d` - dia sem zeros
* tag `$MMMM` - mês em formato completo
* tag `$MMM` - mês em formato curto
* tag `$MM` - mês
* tag `$M` - mês sem zeros
* tag `$yyyy` - ano
* tag `$yy` - ano em formato curto
* tag `$hh` - horas
* tag `$h` - horas sem zeros
* tag `$mm` - minutos
* tag `$m` - minutos sem zeros
* tag `$ss` - segundos
* tag `$s` - segundos sem zeros

**Tempo em Atividade personalizado**

* tag `$dd` - tempo em atividade em dias
* tag `$d` - tempo em atividade em dias sem zeros
* tag `$hh` - tempo em atividade em horas
* tag `$h` - tempo em atividade em horas sem zeros
* tag `$mm` - tempo em atividade em minutos
* tag `$m` - tempo em atividade em minutos sem zeros

**Unidades de Temperatura**

Seleciona as unidades de temperatura. Unidades disponíveis são Celsius, Farenheit e Kelvin.

**Dispositivos de Temperatura**

Lista de dispositovos, a serem observados no rótulo de temperatura (os itens de seleção vêm do `sensors`). A lista de widgets é editável, a tecla delete removerá o item selecionado.

**Pontos de Montagem**

Lista de pontos de montagem, a serem observados no rótulo de hdd (os itens de seleção vêm do `mount`). A lista de widgets é editável, a tecla delete removerá o item selecionado.

**HDD (velocidade)**

Lista de dispositivos HDD, a serem observados no rótulo de hddspeed (os itens de seleção vêm do DataEngine). A lista de widgets é editável, a tecla delete removerá o item selecionado.

**HDD (temperatura)**

Lista de dispositivos HDD, a serem observados no rótulo de hddtemp (os itens de seleção vêm do `find`). A lista de widgets é editável, a tecla delete removerá o item selecionado.

**Diretório de rede**

Caminho para o diretório, que contém a informação dos dispositivos de rede. O padrão é `/sys/class/net`. Requerido para seleção automática de dispositivo de rede.

**Dispositivo de rede**

Usa o dispositivo especificado como ativo. Os itens de seleção vêm do **diretório de rede**. Desativará a seleção automática do dispositivo de rede.

**Dispositivo de bateria**

Arquivo com informações da bateria. O arquivo (`/sys/class/power_supply/BAT0/capacity` por padrão) deve conter somente a carga da bateria em porcentagem.

**Tag do carregador plugado**

Linha a ser mostrada quando o carregador está plugado.

**Tag do carregador desplugado**

Linha a ser mostrada quando o carregador está desplugado.

**Dispositivo carregador**

Arquivo com informações do carregador. O arquivo (`/sys/class/power_supply/AC/online` por padrão) deve conter `1` se o carregador estiver plugado.

Configurações da dica de contexto
---------------------------------

Desde a versão 1.7.0, os rótulos de CPU, frequência da CPU, memória, swap e rede suportam dica de contexto gráfica. Para ativar, certifique-se de ter marcado as caixas necessárias. O número de valores armazenados pode ser configurado nesta aba. Cores de gráficos também são configuráveis.

Configurações de DataEngine
---------------------------

**Comando personalizado**

*NOTA* Isso pode travar o seu computador.

Comandos que serão executados para o rótulo personalizado.

**Dispositivo GPU**

Seleciona um dos dispositivos GPU sportador. `auto` ativará a seleção automática, `disable` desativará a definição dos estados de GPU. O padrão é `auto`.

**HDD**

Seleciona um dos HDDs para o monitor de temperatura do HDD. `all` ativará o monitoramento de todos dispositivos, `disable` desativará o monitoramento para todos dispositivos. O padrão é `all`.

**hddtemp cmd**

Digite um comando que será executado para o hddtemp DataEngine. O padrão é `sudo hddtemp`.

**Endereço MPD**

Endereço do servidor MPD. O padrão é `localhost`.

**Porta MPD**

Porta do servidor MPD. O padrão é `6600`.

**Gerenciador de pacotes**

Lista de comandos a serem executador. O número de linhas nulas é o número de linhas desnecessárias. Por padrão:

* *Arch*: `PKGCMD=pacman -Qu`, `PKGNULL=0`
* *Debian*: `PKGCMD=apt-show-versions -u -b`, `PKGNULL=0`
* *Ubuntu*: `PKGCMD=aptitude search '~U'`, `PKGNULL=0`
* *Fedora*: `PKGCMD=yum list updates`, `PKGNULL=3`
* *FreeBSD*: `PKGCMD=pkg_version -I -l '<'`, `PKGNULL=0`
* *Mandriva*: `PKGCMD=urpmq --auto-select`, `PKGNULL=0`

**Reprodutor de música**

Selecione um dos reprodutores de música suportados para o rótulo de música.

Configuração do DataEngine
--------------------------

Você pode editar a configuração do DataEngine. Ela está em `/usr/share/config/extsysmon.conf` ou `$HOME/share/config/extsysmon.conf` dependendo do tipo de instalação. Remova os comentários das linhas necessárias e edite-as.

Instruções
==========

Dependências
------------

* kdebase-workspace
* kdebindings-python2
* lm_sensors (*para definir a temperatura dos dispositivos*)

Dependências opcionais
----------------------

* sysstat (*para notificações*)
* driver de vídeo proprietário
* hddtemp (certifique-se de que pode ser executado com `sudo` sem senha. Para isso, adicione a seguinte linha ao arquivo `/etc/sudoers`: `$USERNAME ALL=NOPASSWD: /usr/bin/hddtemp`)
* music player (amarok, clementine, mpd or qmmp)

Dependências para compilar
--------------------------

* automoc4
* cmake

Instalação
----------

* baixe o código-fonte
* instale

        mkdir build && cd build
        cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=`kde4-config --localprefix` ../
        make && make install

  Caso queira instalar na `/`:

        mkdir build && cd build
        cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=`kde4-config --prefix` ../
        make && sudo make install

Informações adicionais
======================

TODO (lista de desejos)
-----------------------

Links
-----

* [Homepage](http://arcanis.name/projects/pytextmonitor/)
* Plasmoid na [kde-look](http://kde-look.org/content/show.php/Py+Text+Monitor?content=157124)
* DataEngine na [kde-look](http://kde-look.org/content/show.php/Extended+Systemmonitor+DataEngine?content=158773)
* Pacote para ArchLinux no [AUR](https://aur.archlinux.org/packages/kdeplasma-applets-pytextmonitor/)
