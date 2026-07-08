#include <stdint.h>
#include <stdio.h>

char senhaCorreta[6] = {"123456"};
char userCorreto[10] = {"usuario1"};
char senhaDigitada[6] = {"123456"};

int validarLogin(char *user, char *userValid, char *senha1, char *senhaValid) {
    if (strcmp(user, userValid) == 0 && strcmp(senha1, senhaValid) == 0) {
        return 1; // Login válido
    }
    return 0; // Login inválido
}



int main(void)
{

    // int senhasIguais(int *senha1, int *senha2, int tamanho) {
    //     for (int i = 0; i < tamanho; i++) {
    //         if (senha1[i] != senha2[i]) {
    //             return 0; // Senhas diferentes
    //         }
    //     }
    //     return 1; // Senhas iguais
    // }
    // printf("tamanho da senha: %d\n", sizeof(senhaCorreta) / sizeof(senhaCorreta[0]));
    // printf("tamanho da senha digitada: %d\n", sizeof(senhaDigitada) / sizeof(senhaDigitada[0]));

    if (validarLogin(userCorreto, userCorreto, senhaCorreta, senhaCorreta)) {
        printf("Login válido!\n");
    } else {
        printf("Login inválido!\n");
    }

    return 0;
}