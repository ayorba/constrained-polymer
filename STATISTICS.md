### Statistics to Capture

The radius of gyration:
$$R_g(N)=\sqrt{\frac{1}{N}\sum_{k=1}^N\left|\vec{\mathbf{r}}_k-\vec{\mathbf{r}}_{\text{com}}\right|^2}$$

The average radius of gyration over all subchains of length n:
$$\langle R_g(n)\rangle = \frac{1}{N-n+1}\sum_{i=1}^{N-n+1}R_g(i, i+n-1)$$

and $R_g(i, j)$ is the radius of gyration of a subchain starting at $i$ and ending at $j$. 

$$R_g(i, j)=\left[\frac{1}{j-i+1}\sum^j_{k=i}\left(\vec{\mathbf{r}}_k-\langle\vec{\mathbf{r}}_k\rangle\right)^2\right]^{1/2}$$