import pandas as pd

class Node:
    def __init__(self, player_id, overall_rating):
        self.player_id = player_id
        self.overall_rating = overall_rating
        self.left = None
        self.right = None
        self.height = 1

class AVLTree:
    def __init__(self):
        self.root = None
    
    def insert(self, root, player_id, overall_rating):
        if not root:
            return Node(player_id, overall_rating)
        elif overall_rating < root.overall_rating:
            root.left = self.insert(root.left, player_id, overall_rating)
        else:
            root.right = self.insert(root.right, player_id, overall_rating)
        
        root.height = 1 + max(self.get_height(root.left), 
                            self.get_height(root.right))
        
        balance = self.get_balance(root)
        
        # Casos de rotação
        if balance > 1 and overall_rating < root.left.overall_rating:
            return self.right_rotate(root)
        if balance < -1 and overall_rating >= root.right.overall_rating:
            return self.left_rotate(root)
        if balance > 1 and overall_rating >= root.left.overall_rating:
            root.left = self.left_rotate(root.left)
            return self.right_rotate(root)
        if balance < -1 and overall_rating < root.right.overall_rating:
            root.right = self.right_rotate(root.right)
            return self.left_rotate(root)
            
        return root
    
    def left_rotate(self, z):
        y = z.right
        T2 = y.left
        
        y.left = z
        z.right = T2
        
        z.height = 1 + max(self.get_height(z.left), 
                          self.get_height(z.right))
        y.height = 1 + max(self.get_height(y.left), 
                          self.get_height(y.right))
        return y
    
    def right_rotate(self, z):
        y = z.left
        T3 = y.right
        
        y.right = z
        z.left = T3
        
        z.height = 1 + max(self.get_height(z.left), 
                          self.get_height(z.right))
        y.height = 1 + max(self.get_height(y.left), 
                          self.get_height(y.right))
        return y
    
    def get_height(self, root):
        if not root:
            return 0
        return root.height
    
    def get_balance(self, root):
        if not root:
            return 0
        return self.get_height(root.left) - self.get_height(root.right)
    
    def search_range(self, root, min_rating, max_rating, results):
        if not root:
            return
        
        if min_rating < root.overall_rating:
            self.search_range(root.left, min_rating, max_rating, results)
        
        if min_rating <= root.overall_rating <= max_rating:
            results.append((root.player_id, root.overall_rating))
        
        if max_rating > root.overall_rating:
            self.search_range(root.right, min_rating, max_rating, results)
    
    def inorder_traversal(self, root, result):
        if root:
            self.inorder_traversal(root.left, result)
            result.append((root.player_id, root.overall_rating))
            self.inorder_traversal(root.right, result)

#Dataset
class GerenciadorJogadoresAVL:
    def __init__(self, arquivo_csv):
        self.df = pd.read_csv(arquivo_csv)
        self.avl_tree = AVLTree()
        self.hash_table = {}
        
        # Construir AVL Tree e Hash Table
        for index, row in self.df.iterrows():
            player_id = row['player_fifa_api_id']
            overall_rating = row['overall_rating']
            self.avl_tree.root = self.avl_tree.insert(self.avl_tree.root, player_id, overall_rating)
            self.hash_table[player_id] = row.to_dict()
    
    def buscar_por_id(self, player_id):
        return self.hash_table.get(player_id)
    
    def buscar_por_rating(self, min_rating, max_rating):
        results = []
        self.avl_tree.search_range(self.avl_tree.root, min_rating, max_rating, results)
        return [self.hash_table[player_id] for player_id, _ in sorted(results, key=lambda x: x[1], reverse=True)]
    
    def listar_todos_ordenados(self):
        results = []
        self.avl_tree.inorder_traversal(self.avl_tree.root, results)
        return [self.hash_table[player_id] for player_id, _ in sorted(results, key=lambda x: x[1], reverse=True)]

#MENU
def menu_avl():
    gerenciador = GerenciadorJogadoresAVL('C:/Users/55119/Desktop/Facul/Atividades/3Sem/Estrutura de Dados/Trabalho/SoccerPlayerAttributes/dataset_limpo3.csv')
    
    while True:
        print("\n=== GERENCIADOR DE JOGADORES (AVL) ===")
        print("1. Buscar jogador por ID")
        print("2. Buscar jogadores por faixa de rating")
        print("3. Listar todos os jogadores ordenados por rating")
        print("4. Sair")
        
        opcao = input("Escolha uma opção: ")

        if opcao == '1':
            player_id = int(input("Digite o ID do jogador: "))
            jogador = gerenciador.buscar_por_id(player_id)
            if jogador:
                print(f"\nID: {jogador['player_fifa_api_id']}")
                print(f"Overall Rating: {jogador['overall_rating']}")
                print(f"Pé preferido: {jogador.get('preferred_foot', 'N/A')}")
                print(f"Aceleração: {jogador.get('acceleration', 'N/A')}")
                print(f"Potencial: {jogador.get('potential', 'N/A')}")
                print(f"Ataque: {jogador.get('attacking_work_rate', 'N/A')}")
                print(f"Defesa: {jogador.get('defensive_work_rate', 'N/A')}")
            else:
                print("Jogador não encontrado!")

        elif opcao == '2':
            min_rating = float(input("Rating mínimo (0-100): "))
            max_rating = float(input("Rating máximo (0-100): "))
            jogadores = gerenciador.buscar_por_rating(min_rating, max_rating)
            print(f"\n{len(jogadores)} jogadores encontrados:")
            for jogador in jogadores[:10]:
                print(f"ID: {jogador['player_fifa_api_id']} | Rating: {jogador['overall_rating']} | Pé: {jogador.get('preferred_foot', 'N/A')} | Aceleração: {jogador.get('acceleration', 'N/A')} | Potencial: {jogador.get('potential', 'N/A')} | Ataque: {jogador.get('attacking_work_rate', 'N/A')} | Defesa: {jogador.get('defensive_work_rate', 'N/A')}")

        elif opcao == '3':
            jogadores = gerenciador.listar_todos_ordenados()
            print("\nTop 20 jogadores:")
            for jogador in jogadores[:20]:
               print(f"ID: {jogador['player_fifa_api_id']} | Rating: {jogador['overall_rating']} | Pé: {jogador.get('preferred_foot', 'N/A')} | Aceleração: {jogador.get('acceleration', 'N/A')} | Potencial: {jogador.get('potential', 'N/A')} | Ataque: {jogador.get('attacking_work_rate', 'N/A')} | Defesa: {jogador.get('defensive_work_rate', 'N/A')}")


        elif opcao == '4':
            break
        
        else:
            print("Opção inválida!")
        
        input("\nPressione Enter para continuar...")

if __name__ == "__main__":
    menu_avl()