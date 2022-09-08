#include "spaceship.h"

#include "random_extension.h"
#include "projectile.h"

void Spaceship::OnAttach()
{
    for (auto& pixel : m_pixels)
        m_tempColor.push_back(pixel.m_color);

    AnimationClip clip;
    clip.AddFrame(AnimationFrame({ NULL, FG_YELLOW, {0, 0}}, 0.2f));
    clip.AddFrame(AnimationFrame({ NULL, m_color, {0, 0}}, 0.2f));
    AddAnimationClip("damage_flash", std::move(clip));

    s_explosionSound = std::make_shared<AudioPlayer>("explosion.wav");
    s_explosionSound->SetVolume(0.65f);
    s_explosionSound->m_destroyOnFinish = true;
    m_game->RegisterObject(s_explosionSound);

    s_hitSound = std::make_shared<AudioPlayer>("hit.wav");
    s_hitSound->SetVolume(0.65f);
    m_game->RegisterObject(s_hitSound);
}
void Spaceship::OnDetach()
{
    s_hitSound->Invalidate();
}

void Spaceship::OnUpdate(float elapsed)
{
    if (m_hitpoints <= 0)
    {
        Destroy();
        return;
    }

    if (m_isFiring)
    {
        shootCooldown += elapsed;
        if (shootCooldown >= rateOfFire)
        {
            Shoot();
            shootCooldown = 0;
        }
    }

    AnimatedObject::OnUpdate(elapsed);
}

void Spaceship::OnCollide(GameObject& other)
{
    auto projectile = dynamic_cast<Projectile*>(&other);

    if (projectile)
    {
        if (projectile && projectile->GetOwner().lock().get() != this)
        {
            Play("damage_flash");
            s_hitSound->Play();
        }
    }
}

void Spaceship::Shoot()
{
    auto projectile = CreateProjectile();

    switch (orientation)
    {
    case ATAS:
        projectile->m_rect.position.y = m_rect.position.y - 2;
        projectile->m_rect.position.x = m_rect.position.x + 1;
        projectile->m_velocity.y = -projectileSpeed;
        break;
    case BAWAH:
        projectile->m_rect.position.y = m_rect.position.y + 2;
        projectile->m_rect.position.x = m_rect.position.x + 1;
        projectile->m_velocity.y = projectileSpeed;
        break;
    }

    m_game->RegisterObject(projectile);
}

void Spaceship::Destroy()
{
    s_explosionSound->Play();

    for (auto& pixel : m_pixels)
    {
        for (int i = 0; i < 2; i++)
        {
            auto partikel = std::make_shared<AnimatedObject>();

            partikel->m_rect.position.x = m_rect.position.x + pixel.m_posisi.x;
            partikel->m_rect.position.y = m_rect.position.y + pixel.m_posisi.y;
            partikel->m_destroyOnAnimationEnd = true;

            auto speed = RandomUtils::random(5, 10);
            partikel->m_velocity.x = RandomUtils::random(0, 5) < 3 ? speed : -speed;
            partikel->m_velocity.y = RandomUtils::random(0, 5) < 3 ? speed : -speed;

            AnimationClip clip;
            for (int n = 0; n < 5; n++)
            {
                AnimationFrame frame({ L"O@*^."[n], m_color, { 0, 0 } }, 0.2f);
                clip.AddFrame(std::move(frame));
            }
            partikel->AddAnimationClip("explosion", std::move(clip));

            m_game->RegisterObject(partikel);
            partikel->Play("explosion");
        }
    }

    Invalidate();
}

std::shared_ptr<Projectile> Spaceship::CreateProjectile()
{
    return std::make_shared<Projectile>(weak_from_this());
}
