import pandas as pd

try:
    # Caminho do seu arquivo CSV
    caminho_csv = 'C:/Users/55119/Desktop/Facul/Atividades/3Sem/Estrutura de Dados/Trabalho/SoccerPlayerAttributes/dataset_limpo3.csv'

    # Ler o arquivo CSV
    df = pd.read_csv(caminho_csv)


    # 1. Análise inicial das primeiras linhas com formatação melhorada
    print("=== PRIMEIRAS 5 LINHAS ===")
    pd.set_option('display.max_columns', None)
    print(df.head())

    # 2. Estatísticas básicas por tipo de pé preferido
    print("\n=== ESTATÍSTICAS POR PÉ PREFERIDO ===")
    print(df.groupby('preferred_foot')['overall_rating'].describe())

    print("\nNo geral o pé de domínio do jogador não interfere no seu rating")

    # 3. Top 10 jogadores com maior potencial
    print("\n=== TOP 10 MAIOR POTENCIAL ===")
    print(df[['player_fifa_api_id', 'potential', 'overall_rating']]
      .sort_values('potential', ascending=False)
      .head(10)
      .to_string(index=False))

    # 4. Comparação entre atributos ofensivos

    atributos_ofensivos = ['finishing', 'dribbling', 'shot_power', 'long_shots']
    print("\n=== MÉDIA DE ATRIBUTOS OFENSIVOS ===")
    print(df[atributos_ofensivos].mean())

    print("\nAs habilidades são dadas em uma escala de 0 a 100")

    medias = df[atributos_ofensivos].mean().sort_values(ascending=False)
    print("\nRanking de habilidades ofensivas médias:")
    print(medias)

    print("\n* Um finishing de 65+ já indica um bom finalizador" \
    "\n* Atributos acima de 75 são considerados de elite" \
    "\n* Valores abaixo de 40 são raros para jogadores profissionais")

    print("\nMédia por pé preferido:")
    print(df.groupby('preferred_foot')[atributos_ofensivos].mean())

    #FINISHING
    print("\n")
    df['finishing_category'] = pd.cut(df['finishing'],
                                 bins=[0, 40, 60, 80, 100],
                                 labels=['Ruim', 'Médio', 'Bom', 'Elite'])
    print(df['finishing_category'].value_counts())

    #DRIBBLING
    print("\n")
    df['dribbling_category'] = pd.cut(df['dribbling'],
                                 bins=[0, 40, 60, 80, 100],
                                 labels=['Ruim', 'Médio', 'Bom', 'Elite'])
    print(df['dribbling_category'].value_counts())

    #SHOT_POWER
    print("\n")
    df['shot_power_category'] = pd.cut(df['shot_power'],
                                 bins=[0, 40, 60, 80, 100],
                                 labels=['Ruim', 'Médio', 'Bom', 'Elite'])
    print(df['shot_power_category'].value_counts())

    #LONG_SHOTS
    print("\n")
    df['long_shots_category'] = pd.cut(df['long_shots'],
                                 bins=[0, 40, 60, 80, 100],
                                 labels=['Ruim', 'Médio', 'Bom', 'Elite'])
    print(df['long_shots_category'].value_counts())

    # 5. Visualização rápida de correlações

    # Selecionar APENAS colunas numéricas para a correlação
    numeric_cols = df.select_dtypes(include=['int64', 'float64']).columns

    # Calcular a matriz de correlação apenas com colunas numéricas
    correlation_matrix = df[numeric_cols].corr()

    # Focar na correlação com 'overall_rating'
    rating_correlations = correlation_matrix['overall_rating'].sort_values(ascending=False)

    # Mostrar resultados
    print("\n=== CORRELAÇÕES COM OVERALL_RATING ===")
    print(rating_correlations)

    # Visualização gráfica

    import matplotlib.pyplot as plt
    import seaborn as sns

    # Selecionar apenas colunas numéricas
    numeric_cols = df.select_dtypes(include=['number']).columns
    corr_matrix = df[numeric_cols].corr()

    # Configurar gráfico
    correlation_matrix = df[numeric_cols].corr()
    plt.figure(figsize=(15, 12))
    sns.heatmap(correlation_matrix, annot=True, fmt=".2f", cmap='coolwarm', center=0,
                annot_kws={"size": 8}, cbar_kws={"shrink": 0.8})
    plt.title("Correlações", pad=20)
    plt.xticks(rotation=45, ha='right')
    plt.yticks(rotation=0)
    plt.tight_layout()
    plt.show()

    # Pegar correlações com overall_rating
    rating_corr = corr_matrix['overall_rating'].sort_values(ascending=False)[1:]

    # Plotar as top 15
    plt.figure(figsize=(10, 8))
    rating_corr.head(15).plot(kind='barh', color='skyblue')
    plt.title('Top 15 Atributos Correlacionados com Overall Rating')
    plt.xlabel('Coeficiente de Correlação')
    plt.grid(axis='x', linestyle='--', alpha=0.7)
    plt.show()

    print("O overall_rating (ou avaliação geral) é a métrica mais importante nos jogos da série FIFA/EA Sports FC, " \
    "representando a habilidade geral de um jogador em uma escala padronizada.")

    # 6. Análise de work rates

    # Função para padronizar os valores
    def clean_work_rate(rate):
        if isinstance(rate, str):
            rate = rate.lower().strip()
            if rate in ['high', 'h', 'hi']:
                return 'high'
            elif rate in ['medium', 'med', 'm', 'mid']:
                return 'medium'
            elif rate in ['low', 'l', 'lo']:
                return 'low'
        return rate

    # Aplicar a limpeza
    df['attacking_work_rate'] = df['attacking_work_rate'].apply(clean_work_rate)
    df['defensive_work_rate'] = df['defensive_work_rate'].apply(clean_work_rate)

    # Filtrar apenas os valores padronizados
    valid_rates = ['high', 'medium', 'low']
    attack_rates = df[df['attacking_work_rate'].isin(valid_rates)]['attacking_work_rate']
    defense_rates = df[df['defensive_work_rate'].isin(valid_rates)]['defensive_work_rate']

    # Exibir resultados formatados
    print("=== DISTRIBUIÇÃO PADRONIZADA DE WORK RATES ===")
    print("\nAtaque:")
    print(attack_rates.value_counts().sort_index(ascending=False))

    print("\nDefesa:")
    print(defense_rates.value_counts().sort_index(ascending=False))

    # Gráfico de barras
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 5))

    attack_rates.value_counts().sort_index(ascending=True).plot(
        kind='barh', ax=ax1, title='Ataque', color='green')
    defense_rates.value_counts().sort_index(ascending=True).plot(
        kind='barh', ax=ax2, title='Defesa', color='blue')

    plt.tight_layout()
    plt.show()

except FileNotFoundError:
    print("Erro: Arquivo não encontrado. Verifique o caminho e nome do arquivo.")
except Exception as e:
    print(f"Erro ao carregar o arquivo: {e}")