#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Sistema de Gestão de Caixa — Versão Python
Desenvolvedor: Sistema Sênior
Descrição: Gerencia estoque de cédulas/moedas e realiza decomposição de troco.

Nota: todos os valores internos são armazenados em CENTAVOS (int) para
eliminar erros de ponto flutuante com operações em reais.
"""

import os
import sys
from typing import Optional

# ─── Configuração das denominações ───────────────────────────────────────────

# (valor_em_centavos, rótulo_formatado)
DENOMINACOES: list[tuple[int, str]] = [
    (20000, "R$ 200,00"),
    (10000, "R$ 100,00"),
    ( 5000, "R$  50,00"),
    ( 2000, "R$  20,00"),
    ( 1000, "R$  10,00"),
    (  500, "R$   5,00"),
    (  200, "R$   2,00"),
    (  100, "R$   1,00"),
    (   50, "R$   0,50"),
    (   25, "R$   0,25"),
    (   10, "R$   0,10"),
    (    5, "R$   0,05"),
]

# ─── Classe principal ─────────────────────────────────────────────────────────

class Caixa:
    """Representa o caixa com estoque de cédulas e moedas."""

    def __init__(self) -> None:
        # Mapeia valor_em_centavos → quantidade em estoque
        self.estoque: dict[int, int] = {v: 0 for v, _ in DENOMINACOES}

    # ── Propriedades ──────────────────────────────────────────────────────────

    @property
    def saldo_centavos(self) -> int:
        return sum(v * q for v, q in self.estoque.items())

    @property
    def saldo_reais(self) -> float:
        return self.saldo_centavos / 100.0

    # ── Algoritmo de decomposição (greedy) ────────────────────────────────────

    def decompor(
        self,
        valor_cts: int,
        *,
        simulacao: bool = False,
    ) -> Optional[dict[int, int]]:
        """
        Tenta compor *valor_cts* centavos usando o estoque disponível.

        Retorna um dict {valor_centavos: quantidade_usada} se conseguiu,
        ou None caso o caixa não tenha notas/moedas suficientes.

        Se *simulacao* for True, o estoque NÃO é modificado.
        """
        temp = dict(self.estoque)           # cópia de trabalho
        usados: dict[int, int] = {}
        restante = valor_cts

        for valor, _ in DENOMINACOES:
            if valor > restante:
                continue
            qtd_necessaria = restante // valor
            qtd_usar = min(qtd_necessaria, temp[valor])
            if qtd_usar == 0:
                continue
            usados[valor] = qtd_usar
            temp[valor] -= qtd_usar
            restante -= qtd_usar * valor
            if restante == 0:
                break

        if restante != 0:
            return None  # Não conseguiu compor o valor

        if not simulacao:
            self.estoque = temp  # Confirma a dedução

        return usados


# ─── Utilitários de I/O ──────────────────────────────────────────────────────

def limpar_tela() -> None:
    os.system("cls" if os.name == "nt" else "clear")


def linha(char: str = "─", n: int = 56) -> None:
    print(char * n)


def cabecalho(titulo: str) -> None:
    linha("═")
    print(f"   💰  {titulo}")
    linha("═")


def ler_valor(prompt: str) -> int:
    """Lê um valor monetário em reais e retorna em centavos (int)."""
    while True:
        entrada = input(prompt).strip().replace(",", ".")
        try:
            valor = float(entrada)
            if valor < 0:
                raise ValueError
            return round(valor * 100)
        except ValueError:
            print("  ⚠  Valor inválido. Use apenas números (ex.: 12.50).")


def ler_inteiro(prompt: str) -> int:
    """Lê um inteiro não-negativo."""
    while True:
        entrada = input(prompt).strip()
        try:
            valor = int(entrada)
            if valor < 0:
                raise ValueError
            return valor
        except ValueError:
            print("  ⚠  Informe um número inteiro válido.")


def aguardar() -> None:
    input("\n  Pressione ENTER para continuar...")


def formatar_reais(centavos: int) -> str:
    return f"R$ {centavos / 100:>8.2f}"


def imprimir_decomposicao(usados: dict[int, int]) -> None:
    """Exibe a decomposição formatada."""
    linha("─", 42)
    for valor, _ in DENOMINACOES:
        qtd = usados.get(valor, 0)
        if qtd:
            rotulo = next(r for v, r in DENOMINACOES if v == valor)
            print(f"  {rotulo}  x  {qtd}")
    linha("─", 42)


# ─── Funcionalidade 1: Cadastrar notas/moedas ────────────────────────────────

def cadastrar(caixa: Caixa) -> None:
    limpar_tela()
    cabecalho("CADASTRAR NOTAS E MOEDAS")
    print("\n  Informe a quantidade de cada denominação:\n")
    for valor, rotulo in DENOMINACOES:
        qtd = ler_inteiro(f"  {rotulo}  -> qtd: ")
        caixa.estoque[valor] = qtd
    print("\n  ✅  Caixa atualizado com sucesso!")
    aguardar()


# ─── Funcionalidade 2: Ver caixa ─────────────────────────────────────────────

def ver_caixa(caixa: Caixa) -> None:
    limpar_tela()
    cabecalho("EXTRATO DO CAIXA")
    print(f"\n  {'Denominação':<14} {'Qtd':>6}   {'Subtotal':>12}")
    linha("─")
    for valor, rotulo in DENOMINACOES:
        qtd = caixa.estoque[valor]
        subtotal = valor * qtd
        print(f"  {rotulo:<14} {qtd:>6}   {formatar_reais(subtotal):>12}")
    linha("─")
    print(f"  {'TOTAL EM CAIXA:':<22} {formatar_reais(caixa.saldo_centavos):>12}")
    linha("═")
    aguardar()


# ─── Funcionalidade 3: Simular venda e troco ─────────────────────────────────

def simular_venda(caixa: Caixa) -> None:
    limpar_tela()
    cabecalho("SIMULAR VENDA E TROCO")

    compra = ler_valor("\n  Valor da compra       (R$): ")
    pago   = ler_valor("  Valor pago pelo cliente (R$): ")

    if pago < compra:
        print("\n  ❌  Valor insuficiente! O cliente pagou menos do que o total.")
        aguardar()
        return

    troco = pago - compra
    print(f"\n  Troco a devolver: {formatar_reais(troco)}")

    if troco == 0:
        print("\n  ✅  Sem troco. Venda concluída!")
        aguardar()
        return

    usados = caixa.decompor(troco, simulacao=False)

    if usados is None:
        print("\n  ❌  Caixa sem notas/moedas suficientes para o troco exato.")
        aguardar()
        return

    print("\n  Composição do troco:")
    imprimir_decomposicao(usados)
    print("\n  ✅  Venda e troco processados com sucesso!")
    print("  Caixa atualizado.")
    aguardar()


# ─── Funcionalidade 4: Decompor valor avulso ─────────────────────────────────

def decompor_avulso(caixa: Caixa) -> None:
    limpar_tela()
    cabecalho("DECOMPOR VALOR AVULSO")

    valor = ler_valor("\n  Valor a decompor (R$): ")

    if valor == 0:
        print("\n  ⚠  Informe um valor maior que zero.")
        aguardar()
        return

    # simulacao=True → não altera o estoque
    usados = caixa.decompor(valor, simulacao=True)

    if usados is None:
        print(
            f"\n  ❌  Não é possível compor {formatar_reais(valor)} "
            "com as notas/moedas disponíveis no caixa."
        )
        aguardar()
        return

    print(f"\n  Decomposição de {formatar_reais(valor)}:")
    imprimir_decomposicao(usados)
    print("  (Estoque não foi alterado)")
    aguardar()


# ─── Menu principal ───────────────────────────────────────────────────────────

def menu() -> int:
    limpar_tela()
    cabecalho("SISTEMA DE GESTÃO DE CAIXA  v1.0")
    print()
    print("  [1]  Cadastrar Notas/Moedas")
    print("  [2]  Ver Caixa")
    print("  [3]  Simular Venda e Troco")
    print("  [4]  Decompor Valor Avulso")
    print("  [0]  Sair")
    print()
    return ler_inteiro("  Escolha uma opção: ")


def main() -> None:
    caixa = Caixa()
    acoes = {
        1: lambda: cadastrar(caixa),
        2: lambda: ver_caixa(caixa),
        3: lambda: simular_venda(caixa),
        4: lambda: decompor_avulso(caixa),
    }

    while True:
        opcao = menu()

        if opcao == 0:
            limpar_tela()
            print("  Encerrando o sistema. Até logo! 👋\n")
            sys.exit(0)

        acao = acoes.get(opcao)
        if acao:
            acao()
        else:
            print("  ⚠  Opção inválida.")
            aguardar()


if __name__ == "__main__":
    main()
