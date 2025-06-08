import pandas as pd

try:
    # Caminho do seu arquivo CSV
    caminho_csv = 'C:/Users/55119/Desktop/Facul/Atividades/3Sem/Estrutura de Dados/Trabalho/SoccerPlayerAttributes/dataset_limpo3.csv'

    # Ler o arquivo CSV
    df = pd.read_csv(caminho_csv)

except FileNotFoundError:
    print("Erro: Arquivo não encontrado. Verifique o caminho e nome do arquivo.")
except Exception as e:
    print(f"Erro ao carregar o arquivo: {e}")


import os

class GerenciadorJogadores:
    def __init__(self, arquivo_csv):
        if not os.path.exists(arquivo_csv):
            raise FileNotFoundError(f"Arquivo {arquivo_csv} não encontrado!")
            
        self.df = pd.read_csv(arquivo_csv)
        self.hash_table = {str(row['player_fifa_api_id']): index 
                         for index, row in self.df.iterrows()}
        
    def limpar_tela(self):
        os.system('cls' if os.name == 'nt' else 'clear')
        
    def buscar_jogador(self, id_jogador):
        id_str = str(id_jogador)
        if id_str in self.hash_table:
            index = self.hash_table[id_str]
            return self.df.iloc[index]
        return None
    
    def validar_numero(self, prompt, minimo=0, maximo=100):
        while True:
            try:
                valor = float(input(prompt))
                if minimo <= valor <= maximo:
                    return valor
                print(f"Erro: O valor deve estar entre {minimo} e {maximo}!")
            except ValueError:
                print("Erro: Digite um número válido!")
    
    def validar_pe(self, prompt):
        while True:
            pe = input(prompt).lower()
            if pe in ['right', 'left']:
                return pe
            print("Erro: Digite apenas 'right' ou 'left'!")
    
    def inserir_jogador(self):
        self.limpar_tela()
        print("\n--- INSERIR NOVO JOGADOR ---")
        novo_jogador = {}
        
        # Validação do ID
        while True:
            try:
                novo_jogador['player_fifa_api_id'] = int(input("ID do jogador (número inteiro): "))
                if str(novo_jogador['player_fifa_api_id']) in self.hash_table:
                    print("Erro: ID já existe!")
                    input("Pressione Enter para continuar...")
                    return
                break
            except ValueError:
                print("Por favor, digite um número inteiro válido.")
        
        # Validação dos atributos numéricos
        novo_jogador['overall_rating'] = self.validar_numero("Overall Rating (0-100): ")
        novo_jogador['potential'] = self.validar_numero("Potential (0-100): ")
        novo_jogador['preferred_foot'] = self.validar_pe("Pé preferido (right/left): ")
        
        # Adiciona ao DataFrame e à hash table
        novo_index = len(self.df)
        self.df.loc[novo_index] = novo_jogador
        self.hash_table[str(novo_jogador['player_fifa_api_id'])] = novo_index
        print("\nJogador inserido com sucesso!")
        input("Pressione Enter para continuar...")
    
    def remover_jogador(self, id_jogador):
        id_str = str(id_jogador)
        if id_str in self.hash_table:
            index = self.hash_table[id_str]
            self.df = self.df.drop(index).reset_index(drop=True)
            del self.hash_table[id_str]
            # Reconstruir hash table após remoção
            self.hash_table = {str(row['player_fifa_api_id']): index 
                             for index, row in self.df.iterrows()}
            print("\nJogador removido com sucesso!")
        else:
            print("\nErro: ID não encontrado!")
        input("Pressione Enter para continuar...")
    
    def salvar_csv(self, arquivo_saida=None):
        if arquivo_saida is None:
            arquivo_saida = input("Digite o nome do arquivo para salvar (ou Enter para sobrescrever): ") or 'SoccerPlayerAttributes/dataset_limpo3.csv'
        
        self.df.to_csv(arquivo_saida, index=False)
        print(f"\nDados salvos em {arquivo_saida}")
        input("Pressione Enter para continuar...")

def menu():
    print("\n=== GERENCIADOR DE JOGADORES ===")
    print("1. Buscar jogador por ID")
    print("2. Inserir novo jogador")
    print("3. Remover jogador por ID")
    print("4. Salvar alterações")
    print("5. Sair")
    return input("Escolha uma opção (1-5): ")

def main():
    try:
        arquivo = 'C:/Users/55119/Desktop/Facul/Atividades/3Sem/Estrutura de Dados/Trabalho/SoccerPlayerAttributes/dataset_limpo3.csv'  # Altere para seu arquivo
        gerenciador = GerenciadorJogadores(arquivo)
        
        while True:
            gerenciador.limpar_tela()
            opcao = menu()
            
            if opcao == '1':
                gerenciador.limpar_tela()
                print("--- BUSCAR JOGADOR ---")
                id_jogador = input("Digite o ID do jogador: ")
                jogador = gerenciador.buscar_jogador(id_jogador)
                
                if jogador is not None:
                    print("\nDados do jogador:")
                    print("-"*30)
                    print(f"ID: {jogador['player_fifa_api_id']}")
                    print(f"Overall Rating: {jogador['overall_rating']}")
                    print(f"Potential: {jogador['potential']}")
                    print(f"Pé preferido: {jogador['preferred_foot']}")
                    print("-"*30)
                else:
                    print("\nJogador não encontrado!")
                input("\nPressione Enter para continuar...")
                
            elif opcao == '2':
                gerenciador.inserir_jogador()
                
            elif opcao == '3':
                gerenciador.limpar_tela()
                print("--- REMOVER JOGADOR ---")
                id_jogador = input("Digite o ID do jogador a remover: ")
                gerenciador.remover_jogador(id_jogador)
                
            elif opcao == '4':
                gerenciador.limpar_tela()
                gerenciador.salvar_csv()
                
            elif opcao == '5':
                print("\nSaindo do sistema...")
                break
                
            else:
                print("\nOpção inválida! Digite um número de 1 a 5.")
                input("Pressione Enter para continuar...")
                
    except Exception as e:
        print(f"\nOcorreu um erro: {e}")
        input("Pressione Enter para sair...")

if __name__ == "__main__":
    main()